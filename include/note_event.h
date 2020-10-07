// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Note event definition
// Header file

#include <stdint.h> // Needed for <uint8_t>
#include "utils.h"  // Needed for ^count_set_bits(uint8_t)^

typedef struct _note_event_s {
    uint8_t padding: 3; // 00000XXX
    uint8_t parity: 1;  // 0000X000
    uint8_t value: 3;   // 0XXX0000
    uint8_t state: 1;   // X0000000
} note_event_t;

enum NOTE_STATE_ENUM {
    RELEASED = 0b0,
    PRESSED = 0b1
};

enum NOTE_VALUE_ENUM {
    C = 0b001,
    D = 0b010,
    E = 0b011,
    F = 0b100,
    G = 0b101,
    A = 0b110,
    B = 0b111
};

// Returns a note event crafted from a note state and a note value
note_event_t craft_note(uint8_t state, uint8_t value);

// Returns a uint8 representation of a note event
uint8_t note_event_as_uint8(note_event_t);

// Returns a note event representation of a uint8
note_event_t uint8_as_note_event(uint8_t);

uint8_t get_state(note_event_t);

uint8_t get_value(note_event_t);

uint8_t get_parity(note_event_t);

uint8_t check_parity_bit(note_event_t);