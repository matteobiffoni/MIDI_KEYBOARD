// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// AVR communication protocol implementation
// Source file

#include "communication.h"      // Header file


// Flags to control interrupts
volatile uint8_t history = 0xFF;
volatile uint8_t last = 0x00;
volatile unsigned int notes = 0;

// Initialize the TX communication
void comm_init(void) {
    UCSR0A = (1 << U2X0); // Double speed transmission, no Multi-Process
    UBRR0H = (uint8_t) (UBRR_VALUE >> 8);
    UBRR0L = (uint8_t) UBRR_VALUE;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

// Send a note event through TX
void comm_send(note_event_t event) {
    while(!(UCSR0A & (1 << UDRE0)));
    uint8_t c = note_event_as_uint8(event);
    UDR0 = c;
}

// Interrupt service routine
ISR(PCINT0_vect) {
    uint8_t new = PINB ^ history;
    uint8_t note_state;
    uint8_t note_value;
    char change = 0;
    if(new & (1 << PINB0)) {
        note_value = C;
        if(history & (1 << PINB0)) {
            if((last & (1 << 0)) == 0) {
                note_state = PRESSED;
                last |= (1 << 0);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 0)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 0);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB1)) {
        note_value = D;
        if(history & (1 << PINB1)) {
            if((last & (1 << 1)) == 0) {
                note_state = PRESSED;
                last |= (1 << 1);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 1)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 1);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB2)) {
        note_value = E;
        if(history & (1 << PINB2)) {
            if((last & (1 << 2)) == 0) {
                note_state = PRESSED;
                last |= (1 << 2);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 2)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 2);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB3)) {
        note_value = F;
        if(history & (1 << PINB3)) {
            if((last & (1 << 3)) == 0) {
                note_state = PRESSED;
                last |= (1 << 3);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 3)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 3);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB4)) {
        note_value = G;
        if(history & (1 << PINB4)) {
            if((last & (1 << 4)) == 0) {
                note_state = PRESSED;
                last |= (1 << 4);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 4)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 4);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB5)) {
        note_value = A;
        if(history & (1 << PINB5)) {
            if((last & (1 << 5)) == 0) {
                note_state = PRESSED;
                last |= (1 << 5);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 5)) && notes != 0)  {
                note_state = RELEASED;
                last &= ~(1 << 5);
                notes++;
                change = 1;
            }
        }
    }
    if(new & (1 << PINB6)) {
        note_value = B;
        if(history & (1 << PINB6)) {
            if((last & (1 << 6)) == 0) {
                note_state = PRESSED;
                last |= (1 << 6);
                notes++;
                change = 1;
            }
        }
        else {
            if((last & (1 << 6)) && notes != 0) {
                note_state = RELEASED;
                last &= ~(1 << 6);
                notes++;
                change = 1;
            }
        }
    }
    history = PINB;
    if(change) {
        note_event_t note_event = craft_note(note_state, note_value);
        comm_send(note_event);
    }
}