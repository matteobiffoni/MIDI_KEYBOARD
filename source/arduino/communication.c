// MIDI KEYBOARD -- Matteo Biffoni
// Communication handler source
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "../../include/arduino/communication.h"
#include "../../include/note.h"
#include "../../include/arduino/serial.h"

// Initialization
void communication_init(void) {
    serial_init();

}

// Blocking: send a note, 0 on success, 1 otherwise
uint8_t communication_send(const note_t *n) {
    uint8_t size;
    serial_send(n, size);
    while(serial_send_active);
    
}