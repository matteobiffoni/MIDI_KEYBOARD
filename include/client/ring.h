// MIDI KEYBOARD -- Matteo Biffoni
// Ring buffer header
#ifndef __RING_MODULE_H
#define __RING_MODULE_H

#include <pthread.h>

enum RING_STATE_E {
    SPACE_AVAILABLE = 0,
    FULL
};

typedef struct _ring_s {
    unsigned char *base;
    pthread_mutex_t lock[1];
    size_t first;
    size_t last;
    size_t size;
    unsigned char full;
} ring_t;

// Initialization
ring_t *ring_new(size_t buffer_size);

// Delete
void ring_delete(ring_t*);

// Size
size_t ring_size(ring_t*);

// Current items
size_t ring_used(ring_t*);

// Empty check: 1 if empty, 0 otherwise
unsigned char ring_isEmpty(ring_t*);

// Full check: 1 if full, 0 otherwise
unsigned char ring_isFull(ring_t*);

// Pop: 0 on success, 1 on failure
unsigned char ring_pop(ring_t*, unsigned char *dest);

// Push: 0 on success, 1 on failure
unsigned char ring_push(ring_t*, unsigned char val);

// Flush
void ring_flush(ring_t*);

#endif // __RINGBUFFER_MODULE_H