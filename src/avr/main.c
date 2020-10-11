#include <stdlib.h>

#include "buttons.h"

int main(void) {
    communication_init();
    buttons_init();
    while(1) {
      note_event_t *events = malloc(sizeof(note_event_t) * MAX_EVENTS);
      size_t size = perform_interrupt_debounced_scan(events);
      if(size == -1) {
        // HANDLE ERROR
      }
      uint8_t send = communication_send(events, size);
      if(send != 0) {
        // HANDLE ERROR
      }
      free(events);
    }
}