// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// AVR communication protocol definition
// Source file

#include <avr/io.h>         // Needed for avr stuff
#include <avr/interrupt.h>  // Needed for avr interrupts

#include "note_event.h"     // Needed for ^comm_send(note_event_t)^

// Baud rate definition
#define BAUD_RATE 115200
// Ubbr value definition
// 8 because U2X is enabled
#define UBRR_VALUE (F_CPU / 8 / BAUD_RATE - 1)

// Initialize the TX communication
void comm_init(void);

// Send a note event through TX
void comm_send(note_event_t);