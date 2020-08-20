// MIDI KEYBOARD -- Matteo Biffoni
// Communication header
#ifndef __COMMUNICATION_MODULE_H
#define __COMMUNICATION_MODULE_H

#include "../note.h"
#include <stdint.h>

// Initialization
void communication_init(void);

// Blocking receive: 0 on success, 1 on failure
uint8_t communication_receive(note_t*);

// Blocking send: 0 on success, 1 on failure
uint8_t communication_send(const note_t*);

#endif // __COMMUNICATION_MODULE_H