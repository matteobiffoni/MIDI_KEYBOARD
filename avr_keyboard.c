#include <avr/io.h>
#include <avr/interrupt.h>    // Needed to use interrupts    
#include "avr_common/uart.h"
#include <stdio.h>
#include <stdint.h>
#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1)
volatile uint8_t history = 0xFF;
volatile uint8_t last = 0x00;
volatile unsigned int notes = 0;


void UART_init(void) {
    UBRR0H = (uint8_t) (MYUBRR >> 8);
    UBRR0L = (uint8_t) MYUBRR;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void UART_putChar(uint8_t c) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART_sendNote(uint8_t* note) {
    while(*note) {
        UART_putChar(*note); 
        ++note;
    }
}

int main(void) {
    //printf_init();
    UART_init();
    DDRB &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5) | (1 << DDB6)); // Clear the PB0, PB1, PB2, PB3, PB4, PB5, PB6 pin
    // PB0, PB1, PB2, PB3, PB4, PB5, PB6 are now inputs

    PORTB |= ((1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3) | (1 << PORTB4) | (1 << PORTB5) | (1 << PORTB6)); // turn On the Pull-up
    // PULL_UP ENABLED 

    PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
    PCMSK0 |= ((1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4) | (1 << PCINT5) | (1 << PCINT6));   // set PCINT0 to trigger an interrupt on state change 

    sei();                     // turn on interrupts

    while(1) {
        /*main program loop here */
    }
}



ISR(PCINT0_vect) {
    uint8_t changed = PINB ^ history;
    if(changed & (1 << PINB0)) {
        if(history & (1 << PINB0)) {
            if((last & (1 << 0)) == 0) {
                //printf("C pressed\n");
                UART_sendNote((uint8_t*) "C pressed\n");
                last |= (1 << 0);
                notes++;
            }
        }
        else {
            if((last & (1 << 0)) && notes != 0) {
                //printf("C released\n");
                UART_sendNote((uint8_t*) "C released\n");
                last &= ~(1 << 0);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB1)) {
        if(history & (1 << PINB1)) {
            if((last & (1 << 1)) == 0) {
                //printf("D pressed\n");
                UART_sendNote((uint8_t*) "D pressed\n");
                last |= (1 << 1);
                notes++;
            }
        }
        else {
            if((last & (1 << 1)) && notes != 0) {
                //printf("D released\n");
                UART_sendNote((uint8_t*) "D released\n");
                last &= ~(1 << 1);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB2)) {
        if(history & (1 << PINB2)) {
            if((last & (1 << 2)) == 0) {    
                //printf("E pressed\n");
                UART_sendNote((uint8_t*) "E pressed\n");
                last |= (1 << 2);
                notes++;
            }
        }
        else {
            if((last & (1 << 2)) && notes != 0) {
                //printf("E released\n");
                UART_sendNote((uint8_t*) "E released\n");
                last &= ~(1 << 2);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB3)) {
        if(history & (1 << PINB3)) {
            if((last & (1 << 3)) == 0) {
                //printf("F pressed\n");
                UART_sendNote((uint8_t*) "F pressed\n");
                last |= (1 << 3);
                notes++;
            }
        }
        else {
            if((last & (1 << 3)) && notes != 0) {
                //printf("F released\n");
                UART_sendNote((uint8_t*) "F released\n");
                last &= ~(1 << 3);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB4)) {
        if(history & (1 << PINB4)) {
            if((last & (1 << 4)) == 0) {
                //printf("G pressed\n");
                UART_sendNote((uint8_t*) "G pressed\n");
                last |= (1 << 4);
                notes++;
            }
        }
        else {
            if((last & (1 << 4)) && notes != 0) {
                //printf("G released\n");
                UART_sendNote((uint8_t*) "G released\n");
                last &= ~(1 << 4);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB5)) {
        if(history & (1 << PINB5)) {
            if((last & (1 << 5)) == 0) {
                //printf("A pressed\n");
                UART_sendNote((uint8_t*) "A pressed\n");
                last |= (1 << 5);
                notes++;
            }
        }
        else {
            if((last & (1 << 5)) && notes != 0) {
                //printf("A released\n");
                UART_sendNote((uint8_t*) "A released\n");
                last &= ~(1 << 5);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB6)) {
        if(history & (1 << PINB6)) {
            if((last & (1 << 6)) == 0) {
                //printf("B pressed\n");
                UART_sendNote((uint8_t*) "B pressed\n");
                last |= (1 << 6);
                notes++;
            }
        }
        else {
            if((last & (1 << 6)) && notes != 0) {
                //printf("B released\n");
                UART_sendNote((uint8_t*) "B released\n");
                last &= ~(1 << 6);
                notes++;
            }
        }
    }
    history = PINB;
}