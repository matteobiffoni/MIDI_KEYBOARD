// MIDI KEYBOARD -- Matteo Biffoni
// Serial header
#ifndef __SERIAL_HEADER_H
#define __SERIAL_HEADER_H

#include <stdint.h>

// USART Parameters
#define BAUD_RATE 115200
#define UBRR_VALUE (F_CPU / 8 / BAUD_RATE - 1)

// TX
#define TX_BUFFER_SIZE 32

// RX
#define RX_BUFFER_SIZE 32

// Initialization
void serial_init(void);

// Non-Blocking read: read 'size' bytes and store them in 'buffer'; return number of bytes read
uint8_t serial_read(void *buffer, uint8_t size);

// Blocking read: read 'size' bytes and store them in 'buffer'; return number of bytes read
uint8_t serial_read_blocking(void *buffer, uint8_t size);

// Read character
#define serial_read_getChar(c) serial_read(c, 1)

// Send data from a buffer: data will be copied into another buffer, so that it can be reused immediately
// 0 on success, 1 on failure
uint8_t serial_send(const void *buffer, uint8_t size);

// Number of bytes received
uint8_t serial_read_available(void);

// Number of bytes sent
uint8_t serial_send_sent(void);

// 1 if read/send action is active, 0 otherwise
uint8_t serial_read_active(void);
uint8_t serial_send_active(void);

// Reset operations
void serial_read_reset(void);
void serial_send_reset(void);

#endif // __SERIAL_HEADER_H