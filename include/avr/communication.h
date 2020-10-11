#include <avr/io.h>         // Needed for AVR I/O
#include <avr/interrupt.h>  // Needed for AVR interrupts

#include "note_event.h"

#define BAUD_RATE (115200)
#define UBRR_VALUE (F_CPU / 8 / BAUD_RATE - 1)
#define MAX_EVENTS (7)

void communication_init(void);

uint8_t communication_send(note_event_t*, size_t);

void communication_reset(void);