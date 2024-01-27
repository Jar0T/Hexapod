#include "BLE/BLE.h"
#include <inttypes.h>
#include <cstring>
#include "RCData/RCData.h"
#include "TimeInfo/TimeInfo.h"
#include "ble/gatt-service/battery_service_server.h"
#include <vector>
#include "pico/multicore.h"

#define HEARTBEAT_PERIOD_MS 1000
#define MAX_BUFFER_SIZE 1000

const uint16_t startToken = 0xAAAA;

void stream_sensors();
void remove_data_from_angles_buffer(size_t size);

uint16_t received_rc_data_size = 0;

uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x0D, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'H', 'e', 'x', 'a', 'p', 'o', 'd', ' ', 'v', '1', '.', '0',
    0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xff,
};
const uint8_t adv_data_len = sizeof(adv_data);

btstack_packet_callback_registration_t sm_event_callback_registration;
btstack_timer_source_t heartbeat;

int le_notification_enabled;
hci_con_handle_t con_handle;

uint16_t max_transfer_size = ATT_DEFAULT_MTU;
uint8_t battery_level = 100;
std::vector<uint8_t> angles_buffer;

void le_counter_setup(void) {
    l2cap_init();
    sm_init();
    att_server_init(profile_data, att_read_callback, att_write_callback);

    // register for HCI events
    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(att_packet_handler);

    battery_service_server_init(battery_level);

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *)adv_data);
    gap_advertisements_enable(1);

    // set one-shot timer
    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(&heartbeat);

    sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_ONLY);
    sm_set_authentication_requirements(SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING);
    sm_use_fixed_passkey_in_display_role(123456);

    hci_power_control(HCI_POWER_ON);
}

void heartbeat_handler(struct btstack_timer_source *ts) {
    battery_level -= 5;
    if (battery_level == 0) battery_level = 100;
    battery_service_server_set_battery_value(battery_level);

    // Restart timer
    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    bd_addr_t addr;

    if (packet_type != HCI_EVENT_PACKET)
        return;

    switch (hci_event_packet_get_type(packet)) {
    case HCI_EVENT_LE_META:
        switch (hci_event_le_meta_get_subevent_code(packet)) {
        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
            printf("Connection complete\n");
            con_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
            sm_request_pairing(con_handle);
            break;
        default:
            break;
        }
        break;
    case SM_EVENT_JUST_WORKS_REQUEST:
        printf("Just Works requested\n");
        sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
        break;
    case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
        printf("Confirming numeric comparison: %" PRIu32 "\n", sm_event_numeric_comparison_request_get_passkey(packet));
        sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
        break;
    case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
        printf("Pass Key: %" PRIu32 "\n", sm_event_passkey_display_number_get_passkey(packet));
        break;
    case SM_EVENT_IDENTITY_CREATED:
        sm_event_identity_created_get_identity_address(packet, addr);
        printf("Identity created: type %u address %s\n", sm_event_identity_created_get_identity_addr_type(packet), bd_addr_to_str(addr));
        break;
    case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
        sm_event_identity_resolving_succeeded_get_identity_address(packet, addr);
        printf("Identity resolved: type %u address %s\n", sm_event_identity_resolving_succeeded_get_identity_addr_type(packet), bd_addr_to_str(addr));
        break;
    case SM_EVENT_IDENTITY_RESOLVING_FAILED:
        sm_event_identity_created_get_address(packet, addr);
        printf("Identity resolving failed\n");
        break;
    case SM_EVENT_PAIRING_STARTED:
        printf("Pairing started\n");
        break;
    case SM_EVENT_PAIRING_COMPLETE:
        switch (sm_event_pairing_complete_get_status(packet)) {
        case ERROR_CODE_SUCCESS:
            printf("Pairing completed, success\n");
            break;
        case ERROR_CODE_CONNECTION_TIMEOUT:
            printf("Pairing failed, connection timeout\n");
            break;
        case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
            printf("Pairing failed, disconnected\n");
            break;
        case ERROR_CODE_AUTHENTICATION_FAILURE:
            printf("Pairing failed, authentication failure with reason: %u\n", sm_event_pairing_complete_get_reason(packet));
            break;
        default:
            break;
        }
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        printf("Disconnected\n");
        break;
    default:
        break;
    }
}

void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {
            case ATT_EVENT_CONNECTED:
                max_transfer_size = att_server_get_mtu(att_event_connected_get_handle(packet)) - 3;
                printf("ATT connected. Transfer size: %d\n", max_transfer_size);
                break;
            case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
                max_transfer_size = att_server_get_mtu(att_event_connected_get_handle(packet)) - 3;
                printf("ATT connected. Transfer size: %d\n", max_transfer_size);
                break;
            case ATT_EVENT_CAN_SEND_NOW:
                stream_sensors();
                break;
            case ATT_EVENT_DISCONNECTED:
                le_notification_enabled = false;
                received_rc_data_size = 0;
                angles_buffer.clear();
                printf("ATT disconnected\n");
                break;
            default:
                break;
        }
        break;
    default:
        break;
    }
}

uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    UNUSED(connection_handle);

    return 0;
}

int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    UNUSED(transaction_mode);
    UNUSED(offset);

    size_t copy_size;

    switch (att_handle) {
    case ATT_CHARACTERISTIC_00000002_61d1_11ee_8c99_0242ac120002_01_VALUE_HANDLE:
        copy_size = MIN(buffer_size, sizeof(targetDirection) - received_rc_data_size);
        memcpy(reinterpret_cast<uint8_t*>(&targetDirection) + received_rc_data_size, buffer, copy_size);
        received_rc_data_size += copy_size;
        if (received_rc_data_size >= sizeof(targetDirection)) {
            received_rc_data_size = 0;
            lastUpdateTime = TimeInfo::getInstance().CurrentTime();
            printf("Direction: (%f, %f)\n", targetDirection.x, targetDirection.y);
        }
        break;
    case ATT_CHARACTERISTIC_00000003_61d1_11ee_8c99_0242ac120002_01_VALUE_HANDLE:
        memcpy(reinterpret_cast<uint8_t*>(&targetRotation), buffer, sizeof(targetRotation));
        lastUpdateTime = TimeInfo::getInstance().CurrentTime();
        printf("Rotation: %f\n", targetRotation);
        break;
    case ATT_CHARACTERISTIC_00000004_61d1_11ee_8c99_0242ac120002_01_VALUE_HANDLE:
        memcpy(reinterpret_cast<uint8_t*>(&targetHeight), buffer, sizeof(targetHeight));
        printf("Height: %f\n", targetHeight);
        break;
    case ATT_CHARACTERISTIC_00000002_eeaa_4c7d_9a63_8f41e0f2d3a7_01_VALUE_HANDLE:
        if (buffer_size > 0) reset = buffer[0];
        printf("Reset: %d\n", buffer[0]);
        break;
    case ATT_CHARACTERISTIC_00000002_2ec8_467d_901b_5e81822a6ffe_01_CLIENT_CONFIGURATION_HANDLE:
        le_notification_enabled = little_endian_read_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
        if (le_notification_enabled) {
            con_handle = connection_handle;
            att_server_request_can_send_now_event(con_handle);
            printf("Sensors notifications enabled\n");
        } else {
            angles_buffer.clear();
            printf("Sensors notifications disabled\n");
        }
        break;
    default:
        break;
    }
    return 0;
}

void stream_sensors() {
    if (le_notification_enabled) {
        uint16_t transfer_size = MIN(angles_buffer.size(), max_transfer_size);
        if (transfer_size > 0) {
            att_server_notify(con_handle,
                ATT_CHARACTERISTIC_00000002_2ec8_467d_901b_5e81822a6ffe_01_VALUE_HANDLE,
                angles_buffer.data(),
                transfer_size);
            remove_data_from_angles_buffer(transfer_size);
        }
        att_server_request_can_send_now_event(con_handle);
    }
    return;
}

void add_angles_to_buffer(uint8_t leg_number, Vector3 angles) {
    uint32_t time = TimeInfo::getInstance().CurrentTime();

    size_t size = angles_buffer.size();
    size_t data_size = sizeof(time) + sizeof(leg_number) + sizeof(angles) + 2;

    if (size + data_size < MAX_BUFFER_SIZE) {
        angles_buffer.resize(size + data_size);

        memcpy(angles_buffer.data() + size, &startToken, sizeof(startToken));
        memcpy(angles_buffer.data() + size + 2, &time, sizeof(time));
        memcpy(angles_buffer.data() + size + 2 + sizeof(time), &leg_number, sizeof(leg_number));
        memcpy(angles_buffer.data() + size + 2 + sizeof(time) + sizeof(leg_number), &angles, sizeof(angles));
    }
}

void remove_data_from_angles_buffer(size_t size) {
    if (size > angles_buffer.size()) size = angles_buffer.size();
    angles_buffer.erase(angles_buffer.begin(), angles_buffer.begin() + size);
}