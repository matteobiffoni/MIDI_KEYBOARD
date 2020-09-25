// MIDI KEYBOARD -- Matteo Biffoni
// Communication handler source
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "../../include/arduino/sleep_util.h"
#include "../../include/arduino/communication.h"
#include "../../include/note.h"
#include "../../include/arduino/serial.h"
#include "../../include/command.h"

#define COMMAND_NONE COMMAND_COUNT

static uint8_t note_global_id;

// Timer variables
static const uint16_t rto = 150;
static volatile uint8_t rto_elapsed;
static volatile uint8 rto_active;

static command_id_t command_current = COMMAND_NONE;
static uint8_t command_notified = 0;

static communication_op_f opmode_default[];
static communication_opmode_t opmode = opmode_default;

static inline void command_end(void) {
    command_current = COMMAND_NONE;
}

// Stop the timer
static inline void rto_timer_stop(void) {
    TIMSK3 &= ~(1 << OCIE3A);
    rto_active = 0;
}

// Start the timer
static inline void rto_timer_start(void) {
    if(rto_active) rto_timer_stop();
    rto_active = 1;
    rto_elapsed = 0;
    TCNT3 = 0;
    TIMSK3 |= (1 << OCIE3A);
}

// Timer ISR
ISR(TIMER3_COMPA_vect) {
    rto_timer_stop();
    rto_elapsed = 1;
}

// Initialization
void communication_init(void) {
    serial_init();

    TCCR3A = 0;
    TCCR3B = (1 << WGM52) | (1 << CS50) | (1 << CS52);
    OCR3A = (uint16_t) (15.625 * rto);

    note_global_id = 0;
    rto_elapsed = 0;
    rto_active = 1;
}

// Single attempt to receive a note: 0 on success, 1 otherwise
static uint8_t _receive_attempt(note_t *n) {
    uint8_t *n_raw = (uint8_t*) n;
    if(!rto_active) return 1;
    uint8_t type, id, size = 0; received = 0;
    while(1) {
        if(rto_elapsed) return E_TIMEOUT_ELAPSED;
        if(serial_read_getChar(n_raw + received)) {
            switch (++received) {
                case 1:
                    type = note_get_type(n);
                    id = note_get_id(n);
                    if((type == NOTE_TYPE_HND && id != 0) && id != note_global_id) return E_ID_MISMATCH;
                    break;
                case 2:
                    if(note_check_header(n) != 0) return E_CORRUPTED_HEADER;
                    size = note_get_size(n);
                    break;
                default:
                    if(received >= size) return (note_check_crc(n) != 0) ? E_CORRUPTED_CRC : E_SUCCESS;
                    break;
            }
        }
    }
}

// Blocking receive: 0 on success, 1 otherwise
uint8_t communication_receive(note_t *n) {
    static note_t response[1];
    for(uint8_t attempt = 0; attempt < MAX_ATTEMPTS_RECEIVE; ++attempt) {
        rto_timer_start();
        uint8_t ret = _receive_attempt(n);
        if(ret == E_SUCCESS) {
            note_ack(n, response);
            serial_send(response, NOTE_MIN_SIZE);
            rto_timer_stop();
            if(note_get_type(n) == NOTE_TYPE_HND) 
                note_global_id = 1;
            else note_global_id = note_next_it(note_global_id);
            return 0;
        }
        else if(ret == E_TIMEOUT_ELAPSED) continue;
        note_err(n, response);
        serial_send(response, NOTE_MIN_SIZE);
        while(1) {
            serial_read_reset();
            sleep_on(SLEEP_MODE_IDLE, !rto_elapsed);
            id(rto_elapsed) break;
        }
    }
    note_global_id = 0;
    return 1;
}

// Blocking: send a note, 0 on success, 1 otherwise
uint8_t communication_send(const note_t *n) {
    const uint8_t size = note_get_size(n);
    if(!n || !size || size > sizeof(note_t)) return 1;
    for(uint8_t attempt = 0; attempt < MAX_ATTEMPTS_SEND; ++attempt) {
        rto_timer_start();
        serial_send(n, size);
        while(serial_send_active());
        static note_t response[1];
        uint8_t ret = _receive_attempt(response);
        if(ret == E_SUCCESS && note_get_type(response) == NOTE_TYPE_ACK) {
            rto_timer_stop();
            note_global_id = note_next_id(note_global_id);
            command_notified = 1;
            return 0;
        }
        else if(ret != E_TIMEOUT_ELAPSED) {
            sleep_while(SLEEP_MODE_IDLE, !rto_elapsed);
        }
    }
    note_global_id = 0;
    return 1;
}

// In-place note send: 0 on success, 1 otherwise
uint8_t communication_craft_and_send(note_type_t type, const uint8_t *data, uint8_t data_size) {
    static note_t n[1];
    if(note_craft(note_global_id, type, data, data_size, n) != 0) return 1;
    return communication_send(n);
}

// Communication
uint8_t communication_handler(void) {
    uint8_t ret = 0;
    if(command_notified) {
        command_notified = 0;
        if(command_iterate(command_current, NULL) != CMD_RET_ACTIVE) command_end();
        ret = 1;
    }
    if(!serial_read_available()) return ret;

    static note_t n[1];
    if(communication_receive(n) != 0) return 1;
    const uint8_t type = note_get_type(n);
    communication_op_f action = opmode[type] ? opmode[type] : opmode_default[type];
    if(action && action(n) != CMD_RET_ACTIVE) communication_opmode_restore();
    return 1;
}

void communication_opmode_switch(const communication_opmode_t opmode_new) {
    if(opmode_new) opmode = opmode_new;
}

void communication_opmode_restore(void) {
    opmode = opmode_default;
}

static uint8_t _op_hnd(const note_t *read_note) {
    communication_opmode_restore();
    return CMD_RET_ACTIVE;
}

static uint8_t _op_cmd(const note_t *read_note) {
    const command_payload_t *payload = (const command_payload_t*) read_note->data;
    command_id_t cmd = payload->id;
    uint8_t ret = command_execute(cmd);
    if(ret == CMD_RET_ACTIVE) command_current = cmd;
    return ret;
}

static communication_op_f opmode_default[] {
    _op_hnd, NULL, NULL, _op_cmd, NULL, NULL
};