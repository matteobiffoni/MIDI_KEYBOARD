#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../../avr_common/uart.h"

#include "communication.h"

static inline void avr_setup(void) {
    DDRB &= ~(
        (1 << DDB0) | 
        (1 << DDB1) | 
        (1 << DDB2) | 
        (1 << DDB3) | 
        (1 << DDB4) | 
        (1 << DDB5) | 
        (1 << DDB6)
    );
    PORTB |= (
        (1 << PORTB0) |
        (1 << PORTB1) |
        (1 << PORTB2) |
        (1 << PORTB3) |
        (1 << PORTB4) |
        (1 << PORTB5) |
        (1 << PORTB6)
    );
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (
        (1 << PCINT0) |
        (1 << PCINT1) |
        (1 << PCINT2) |
        (1 << PCINT3) |
        (1 << PCINT4) |
        (1 << PCINT5) |
        (1 << PCINT6)
    );
    sei();
}

int main(void) {
    comm_init();
    avr_setup();
    while(1);
}