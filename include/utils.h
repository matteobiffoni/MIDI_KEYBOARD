// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Utils definition
// Header file

#include <stdint.h>     // Needed for <uint8_t>
#include <stdio.h>      // Needed for ^printf()^

enum QUIT_STATE_E {
    NOT_QUIT = 0,
    QUIT_OK,
    QUIT_PARITY_ERROR,
    QUIT_FLAGS_ERROR,
    QUIT_DEVICE_OPEN_FAILURE,
    QUIT_AUDIO_CONTEXT_CREATION_ERROR,
    QUIT_AUDIO_CONTEXT_SETTING_ERROR,
    QUIT_MEMORY_ALLOCATOR_ERROR,
    QUIT_THREAD_CREATION_ERROR,
    QUIT_THREAD_JOINING_ERROR,
    QUIT_UNKNOWN_ERROR
};


extern int quit_state;

// Prints a byte in binary representation
void print_byte(uint8_t);

// Returns the parity bit for given data
uint8_t compute_parity_bit(uint8_t data);

// Returns the n-th bit of byte
//uint8_t get_nth_bit(uint8_t u, uint8_t n);

