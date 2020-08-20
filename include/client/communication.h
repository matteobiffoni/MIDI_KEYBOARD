// MIDI KEYBOARD -- Matteo Biffoni
// Communication header
#ifndef __COMMUNICATION_MODULE_H
#define __COMMUNICATION_MODULE_H

#include "../note.h"
#include "serial.h"

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

#endif // __COMMUNICATION_MODULE_H