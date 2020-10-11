#include "buttons.h"

uint8_t key_state;      // debounced and inverted key state
uint8_t key_press;      // key press detect
uint8_t key_release;    // key release detect

ISR(TIMER0_COMPA_vect) {
    // every 10ms
    // 8 * 2bit counters
    static uint8_t ct0 = 0xFF, ct1 = 0xFF;        
    uint8_t i;
    // key changed ?
    i = key_state ^ ~KEY_PIN;
    // reset or count ct0
    ct0 = ~( ct0 & i );
    // reset or count ct1
    ct1 = ct0 ^ (ct1 & i);
    // count until roll over ?
    i &= ct0 & ct1;
    key_release |= key_state & i;
    // then toggle debounced state
    key_state ^= i;
    // 0->1: key press detect
    key_press |= key_state & i;
}

void buttons_init() {
    DDRB &= ~(
        (1 << DDB0) | 
        (1 << DDB1) | 
        (1 << DDB2) | 
        (1 << DDB3) | 
        (1 << DDB4) | 
        (1 << DDB5) | 
        (1 << DDB6)
    );
    PORTB |= (
        (1 << PORTB0) |
        (1 << PORTB1) |
        (1 << PORTB2) |
        (1 << PORTB3) |
        (1 << PORTB4) |
        (1 << PORTB5) |
        (1 << PORTB6)
    );
    // Mode 2: CTC
    TCCR0A = 1<<WGM01;
    // divide by 1024
    TCCR0B = 1<<CS02^1<<CS00;
    // 10ms                   
    OCR0A = XTAL / 1024.0 * 10e-3 - 1;
    // enable T0 interrupt
    TIMSK0 = 1<<OCIE0A;
}

uint8_t get_key_press( uint8_t key_mask ) {
    // read and clear atomic !
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        // read key(s)
        key_mask &= key_press;
        // clear key(s)
        key_press ^= key_mask;                      
    }
    return key_mask;
}

uint8_t get_key_release( uint8_t key_mask ) {
    // read and clear atomic
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        // read key(s)
        key_mask &= key_release;
        // clear key(s)
        key_release ^= key_mask;
    }
    return key_mask;
}

size_t perform_interrupt_debounced_scan(note_event_t* buffer) {
    if(!buffer) return -1;
    size_t size = 0;
    for(uint8_t i = 0; i < NOTES_COUNT; i++) {
        if(get_key_press(1 << i))
            buffer[size++] = craft_note(PRESSED, i + 1);
        if(get_key_release(1 << i))
            buffer[size++] = craft_note(RELEASED, i + 1);
    }
    return size;
}