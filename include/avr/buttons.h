#include <util/atomic.h>

#include "communication.h"

// 8MHz
#define XTAL            8e6

#define KEY_PIN         PINB

#define NOTES_COUNT     (7)

void buttons_init(void);

uint8_t get_key_press(uint8_t);

uint8_t get_key_release(uint8_t);

size_t perform_interrupt_debounced_scan(note_event_t*);