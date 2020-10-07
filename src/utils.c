// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Utils implementation
// Source file

#include "utils.h"

int quit_state = NOT_QUIT;

void print_byte(uint8_t u) {
    uint8_t z = 128;
    while(z > 0) {
        if(u & z) printf("1");
        else printf("0");
        z >>= 1;
    }
    printf("\n");
}

uint8_t compute_parity_bit(uint8_t data) {
    uint8_t parity=0;
    while(data){
        parity ^= (data & 1);
        data >>= 1;
    }
    return parity;
}

/*uint8_t get_nth_bit(uint8_t u, uint8_t n) {
    return (u << n) & 1;
}*/