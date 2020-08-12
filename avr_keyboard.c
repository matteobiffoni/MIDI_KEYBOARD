#include <avr/io.h>
#include <avr/interrupt.h>    // Needed to use interrupts    
#include "avr_common/uart.h"
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
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
uint8_t getChar(void) {
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}
uint8_t* getNote(uint8_t* buffer) {
    uint8_t* b0 = buffer;
    while(1) {
        uint8_t c = getChar();
        *buffer = c;
        ++buffer;
        if(c == 0)
            return buffer - b0;
        if(c == '\n' || c == '\r') {
            *buffer = 0;
            ++buffer;
            return buffer - b0;
        }
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

    TCCR4A = 0;
    TCCR4B = (1 << CS42) | (1 << CS40);
    
    char str[128];
    while(1) {
        /*main program loop here */
        /*uint16_t timer_val = TCNT4;
        sprintf(str, "Time: %u\n", timer_val);
        UART_sendNote((uint8_t*) str);
        _delay_ms(1000);*/
    }
}



ISR(PCINT0_vect) {
    uint8_t changed = PINB ^ history;
    uint16_t timer_val = TCNT4;
    char msg[128];
    if(changed & (1 << PINB0)) {
        if(history & (1 << PINB0)) {
            if((last & (1 << 0)) == 0) {
                //printf("C pressed\n");
                sprintf(msg, "C pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 0);
                notes++;
            }
        }
        else {
            if((last & (1 << 0)) && notes != 0) {
                //printf("C released\n");
                sprintf(msg, "C released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 0);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB1)) {
        if(history & (1 << PINB1)) {
            if((last & (1 << 1)) == 0) {
                //printf("D pressed\n");
                sprintf(msg, "D pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 1);
                notes++;
            }
        }
        else {
            if((last & (1 << 1)) && notes != 0) {
                //printf("D released\n");
                sprintf(msg, "D released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 1);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB2)) {
        if(history & (1 << PINB2)) {
            if((last & (1 << 2)) == 0) {    
                //printf("E pressed\n");
                sprintf(msg, "E pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 2);
                notes++;
            }
        }
        else {
            if((last & (1 << 2)) && notes != 0) {
                //printf("E released\n");
                sprintf(msg, "E released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 2);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB3)) {
        if(history & (1 << PINB3)) {
            if((last & (1 << 3)) == 0) {
                //printf("F pressed\n");
                sprintf(msg, "F pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 3);
                notes++;
            }
        }
        else {
            if((last & (1 << 3)) && notes != 0) {
                //printf("F released\n");
                sprintf(msg, "F released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 3);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB4)) {
        if(history & (1 << PINB4)) {
            if((last & (1 << 4)) == 0) {
                //printf("G pressed\n");
                sprintf(msg, "G pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 4);
                notes++;
            }
        }
        else {
            if((last & (1 << 4)) && notes != 0) {
                //printf("G released\n");
                sprintf(msg, "G released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 4);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB5)) {
        if(history & (1 << PINB5)) {
            if((last & (1 << 5)) == 0) {
                //printf("A pressed\n");
                sprintf(msg, "A pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 5);
                notes++;
            }
        }
        else {
            if((last & (1 << 5)) && notes != 0) {
                //printf("A released\n");
                sprintf(msg, "A released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 5);
                notes++;
            }
        }
    }
    if(changed & (1 << PINB6)) {
        if(history & (1 << PINB6)) {
            if((last & (1 << 6)) == 0) {
                //printf("B pressed\n");
                sprintf(msg, "B pressed at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last |= (1 << 6);
                notes++;
            }
        }
        else {
            if((last & (1 << 6)) && notes != 0) {
                //printf("B released\n");
                sprintf(msg, "B released at %u ms\n", timer_val);
                UART_sendNote((uint8_t*) msg);
                last &= ~(1 << 6);
                notes++;
            }
        }
    }
    history = PINB;
}