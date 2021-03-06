// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Note event implementation
// Source file

#include "note_event.h"     // Handler file

// Returns a note event crafted from a note state and a note value
note_event_t craft_note(uint8_t state, uint8_t value) {
    note_event_t event;
    event.state = state;
    event.value = value;
    event.padding = 0;
    uint8_t parity = compute_parity_bit(note_event_as_uint8(event));
    event.parity = parity;
    return event;
}

// Returns a uint8 representation of a note event
uint8_t note_event_as_uint8(note_event_t event) {
    uint8_t ret;
    memcpy(&ret, &event, sizeof(event));
    return ret;
}

// Returns a note event representation of a uint8
note_event_t uint8_as_note_event(uint8_t u) {
    note_event_t ret;
    memcpy(&ret, &u, sizeof(u));
    return ret;
}

// Returns the state of a given note event
uint8_t get_state(note_event_t event) {
    return event.state;
}

// Returns the value of a given note event
uint8_t get_value(note_event_t event) {
    return event.value;
}

// Returns the parity bit computed for a given note event
uint8_t get_parity(note_event_t event) {
    return event.parity;
}

// Check the consistence of the parity bit of a given note event
uint8_t check_parity_bit(note_event_t event) {
    uint8_t parity_got = get_parity(event);
    event.parity = 0;
    uint8_t parity_expected = compute_parity_bit(note_event_as_uint8(event));
    return !(parity_got == parity_expected);
}