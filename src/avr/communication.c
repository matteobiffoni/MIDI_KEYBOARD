#include "communication.h"

static inline void tx_sei(void) { UCSR0B |= (1 << TXCIE0); }
static inline void tx_cli(void) { UCSR0B &= ~(1 << TXCIE0); }

static note_event_t tx_buffer[MAX_EVENTS];
static uint8_t tx_to_transmit;
static volatile uint8_t tx_transmitted;
static volatile uint8_t tx_ongoing;

ISR(USART0_TX_vect) {
    ++tx_transmitted;
    if(tx_transmitted < tx_to_transmit)
        UDR0 = note_event_as_uint8(tx_buffer[tx_transmitted]);
    else
        communication_reset();
}

void communication_init(void) {
    // Set baud rate
    UCSR0A = (1 << U2X0); // Double speed transmission, no Multi-Process
    UBRR0H = (uint8_t) (UBRR_VALUE >> 8);
    UBRR0L = (uint8_t) UBRR_VALUE;

    // Set the communication frame width (8 bits for us)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enable TX
    // UDRE interrupts are enabled dynamically by 'communication_send()'
    UCSR0B = (1 << TXEN0);
}

uint8_t communication_send(note_event_t* buf, size_t size) {
    if(!buf || !size || size > MAX_EVENTS) return 1;
    while(tx_ongoing);
    tx_ongoing = 1;
    tx_to_transmit = size;
    tx_transmitted = 0;
    for(uint8_t i = 0; i < size; i++)
        tx_buffer[i] = buf[i];
    tx_sei();
    UDR0 = note_event_as_uint8(tx_buffer[0]);
    return 0;
}

void communication_reset(void) {
    tx_cli();
    tx_to_transmit = 0;
    tx_transmitted = 0;
    tx_ongoing = 0;
}

