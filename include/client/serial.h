// MIDI KEYBOARD -- Matteo Biffoni
// Serial header
#ifndef __SERIAL_MODULE_H
#define __SERIAL_MODULE_H

#include <unistd.h>
#include "../note.h"
#include "ring.h"
#include <pthread.h>

#define BAUD_RATE B115200
#define RX_BUFFER_SIZE 64

typedef struct _serial_context_s {
    int dev_fd;
    struct {
        ring_t *buffer;
        pthread_t thread;
        unsigned char active;
    } rx;
} serial_context_t;

// Open a serial device: return a pointer on success, NULL on failure
serial_context_t *serial_open(const char *dev);

// Close a serial device: 0 on success, 1 on failure
int serial_close(serial_context_t*);

// Read up to 'size' bytes from the serial port: return the number of bytes read
size_t serial_read(serial_context_t*, void *buffer, size_t size);

// Get a single character
#define serial_read_getChar(ctx, dst) serial_read(ctx, dst, 1)

// Write data to the serial port: returns the number of bytes written or -1
ssize_t serial_send(serial_context_t*, const void *buffer, size_t size);

// Put a single character
#define serial_send_putChar(ctx, src) serial_send(ctx, srx, 1)

// Get number of available data
size_t serial_read_available(serial_context_t*);

unsigned char serial_read_active(serial_context_t*);

// Flush
void serial_read_flush(serial_context_t*);

#endif // __SERIAL_MODULE_H