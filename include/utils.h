// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Utils definition
// Header file

#include <stdint.h>     // Needed for <uint8_t>
#include <stdio.h>      // Needed for ^printf()^

// Prints a byte in binary representation
void print_byte(uint8_t);

// Returns the parity bit for given data
uint8_t compute_parity_bit(uint8_t data);

