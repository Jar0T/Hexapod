#include "BLE/BLE.h"
#include <inttypes.h>
#include <cstring>
#include "RCData/RCData.h"
#include "TimeInfo/TimeInfo.h"

#define HEARTBEAT_PERIOD_MS 1000

uint16_t received_rc_data_size = 0;

uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x0D, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'H', 'e', 'x', 'a', 'p', 'o', 'd', ' ', 'v', '1', '.', '0',
    0x03, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x10, 0xff,
};
const uint8_t adv_data_len = sizeof(adv_data);

int le_notification_enabled;
btstack_timer_source_t heartbeat;
btstack_packet_callback_registration_t sm_event_callback_registration;
hci_con_handle_t con_handle;

void le_counter_setup(void) {
    l2cap_init();
    sm_init();
    att_server_init(profile_data, att_read_callback, att_write_callback);

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *)adv_data);
    gap_advertisements_enable(1);

    // register for HCI events
    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(packet_handler);

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
        received_rc_data_size = 0;
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
    default:
        break;
    }
    return 0;
}