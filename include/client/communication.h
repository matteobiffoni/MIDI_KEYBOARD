// MIDI KEYBOARD -- Matteo Biffoni
// Communication header
#ifndef __COMMUNICATION_MODULE_H
#define __COMMUNICATION_MODULE_H

#include "../note.h"
#include "serial.h"
#include "../command.h"

#define RTO_VALUE_MSEC 150

#define MAX_SEND_ATTEMPTS 3
#define MAX_RECV_ATTEMPTS 3

typedef enum ERR_CODE_E {
    E_SUCCESS = 0,
    E_TIMEOUT_ELAPSED,
    E_CORRUPTED_HEADER,
    E_CORRUPTED_CRC,
    E_ID_MISMATCH
} err_code_t;

// Initialization
int communication_init(void);

// Clean-up
void communication_cleanup(void);

// Connection: return 0 on success, 1 on failure
int communication_connect(serial_context_t*);

// Send operation: 0 on success, 1 on failure
int communication_send(serial_context_t*, const note_t*);

// Receive operation: 0 on success, 1 on failure
int communication_receive(serial_context_t*, note_t*);

// Craft an in-place note and send it: 0 on success, 1 on failure
int communication_craft_and_send(serial_context_t*, unsigned char type, const unsigned char *data, unsigned char data_size);

// Craft a command note: 0 on success, 1 otherwise
int communication_cmd(serial_context_t*, command_id_t cmd);

#endif // __COMMUNICATION_MODULE_H