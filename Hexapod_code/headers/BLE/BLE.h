#pragma once

#include "pico/stdlib.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "Vector/Vector3.h"
#include "ble_hexapod.h"

void le_counter_setup(void);
void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
int att_write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
void heartbeat_handler(struct btstack_timer_source *ts);
void add_angles_to_buffer(uint8_t leg_number, Vector3 angles);
