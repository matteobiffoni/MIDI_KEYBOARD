// MIDI KEYBOARD -- Matteo Biffoni
// Communication header
#ifndef __COMMUNICATION_MODULE_H
#define __COMMUNICATION_MODULE_H

#include "../note.h"
#include <stdint.h>

// Number of attempts
#define MAX_ATTEMPTS_SEND 3
#define MAX_ATTEMPTS_RECEIVE 3

// Error codes
typedef enum ERR_CODE_E {
    E_SUCCESS = 0,
    E_TIMEOUT_ELAPSED,
    E_CORRUPTED_HEADER,
    E_CORRUPTED_CRC,
    E_ID_MISMATCH
} err_code_t;

// Communication operation
typedef uint8_t (*communication_op_f) (const note_t *read_note);
typedef communication_op_f communication_opmode_t;

// Initialization
void communication_init(void);

// Blocking receive: 0 on success, 1 on failure
uint8_t communication_receive(note_t*);

// Blocking send: 0 on success, 1 on failure
uint8_t communication_send(const note_t*);

// In-place note send: 0 on success, 1 otherwise
uint8_t communication_craft_and_send(note_type_t type, const uint8_t *data, uint8_t data_size);

// Perform a single iteration
uint8_t communication_handler(void);

// Switch communication opmode
void communication_opmode_switch(const communication_opmode_t opmode_new);

// Restore the communication opmode
void communication_opmode_restore(void);

#endif // __COMMUNICATION_MODULE_H