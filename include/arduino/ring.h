// MIDI KEYBOARD -- Matteo Biffoni
// Ring buffer header
#ifndef __RING_MODULE_H
#define __RING_MODULE_H

#include <stdint.h>

enum RING_STATE_E {
    SPACE_AVAILABLE = 0,
    FULL
}

typedef _ring_s {
    uint8_t *base;
    uint8_t first;
    uint8_t last;
    uint8_t size;
    uint8_t full;
} ring_t;

// Initialization
uint8_t ring_new(ring_t*, uint8_t *buffer, uint8_t size);

// Size
uint8_t ring_size(ring_t*);

// Present items
uint8_t ring_used(ring_t*);

// Empty check: 1 if empty, 0 otherwise
uint8_t ring_isEmpty(ring_t*);

// Full check: 1 if full, 0 otherwise
uint8_t ring_isFull(ring_t*);

// Pop an element: 0 on success, 1 otherwise
uint8_t ring_pop(ring_t*, uint8_t *dest);

// Push an element: 0 on success, 1 otherwise
uint8_t ring_push(ring_t*, uint8_t val);

// Flush operation
void ring_flush(ring_t*);

#endif // __RING_MODULE_H
