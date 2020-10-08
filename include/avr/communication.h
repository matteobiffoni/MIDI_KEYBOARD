#include <avr/io.h>
#include <avr/interrupt.h>

#include "note_event.h"

#define BAUD_RATE 115200
// 8 BECAUSE U2X IS ENABLED
#define UBRR_VALUE (F_CPU / 8 / BAUD_RATE - 1)

void comm_init(void);

void comm_send(note_event_t);