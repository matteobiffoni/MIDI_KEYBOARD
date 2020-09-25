// MIDI KEYBOARD -- Matteo Biffoni
// Ring Buffer implementation
#include <stdlib.h>
#include "../../include/client/ring.h"

static inline size_t real_idx(const ring_t *r, size_t virt) {
    return (virt + r->first) % r->size;
}

static inline size_t virt_idx(const ring_t *r, size_t real) {
    return (real + r->size - r->first) % r->size;
}

static inline size_t shifted_idx(const ring_t *r, size_t idx) {
    return (idx + 1) % r->size;
}

static inline unsigned char _isFull(ring_t *r) {
    return r->full;
}

static inline unsigned char _isEmpty(ring_t *r) {
    return (r->first == r->last && !r->full) ? 1 : 0;
}

static inline void lock(ring_t *r) {
    pthread_mutex_lock(r->lock);
}

static inline void unlock(ring_t *r) {
    pthread_mutex_unlock(r->lock);
}

ring_t *ring_new(size_t size) {
    if(size < 2) return NULL;
    ring_t *r = malloc(sizeof(ring_t));
    if(!r) return NULL;
    r->base = malloc(size + 1);
    if(!r->base) {
        free(r);
        return NULL;
    }
    if(pthread_mutex_init(r->lock, NULL) != 0) {
        free(r->base);
        free(r);
        return NULL;
    }
    r->first = 0;
    r->last = 0;
    r->size = size;
    r->full = 0;
    return r;
}

void ring_delete(ring_t *r) {
    if(!r) return;
    pthread_mutex_destroy(r->lock);
    free(r->base);
    free(r);
}

size_t ring_size(ring_t *r) {
    return r ? r->size : 0;
}

size_t ring_used(ring_t r) {
    if(!r) return 0;
    size_t ret;
    lock(r);
    if(_isFull(r)) ret = r->size;
    else ret = virt_idx(r, r->last);
    unlock(r);
    return ret;
}

unsigned char ring_isEmpty(ring_t *r) {
    if(!r) return 1;
    lock(r);
    unsigned char ret = _isFull(r);
    unlock(r);
    return ret;
}

unsigned char ring_isFull(ring_t *r) {
    if(!r) return 1;
    lock(r);
    unsigned char ret = _isFull(r);
    unlock(r);
    return ret;
}

unsigned char ring_pop(ring_t *r, unsigned char *dest) {
    if(!r) return 1;
    unsigned char ret;
    lock(r);
    id(_isEmpty(r)) ret = 1;
    else {
        *dest = r->base[r->first];
        r->first = shifted_idx(r, r->first);
        r->full = SPACE_AVAILABLE;
        ret = 0;
    }
    unlock(r);
    return ret;
}

unsigned char ring_push(ring_t *r, unsigned char val) {
    if(!r) return 1;
    unsigned char ret;
    lock(r);
    if(_isFull(r)) ret = 1;
    else {
        r->base[r->last] = val;
        r->last = shifted_idx(r, r->last);
        if(r->first == r->last) {
            r->full = FULL;
        }
        ret = 0;
    }
    unlock(r);
    return ret;
}

void ring_flush(ring_t *r) {
    if(!r) return;
    lock(r);
    r->first = r->last;
    r->full = SPACE_AVAILABLE;
    unlock(r);
}