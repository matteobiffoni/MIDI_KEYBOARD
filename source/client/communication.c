// MIDI KEYBOARD -- Matteo Biffoni
// Communication implementation (client)
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "../../include/client/communication.h"
#include "../../include/client/serial.h"
#include "../../include/note.h"

#define ONE_MSEC 1000000

typedef struct _timer_entity_s {
    struct sigevent sig_ev;
    struct sigaction sig_act, sig_act_old;
    struct itimerspec timeval;
    timer_t id;
    volatile unsigned char elapsed;
    volatile unsigned char active;
} timer_entity_t;

static timer_entity_t rto_timer;

static inline int rto_timer_init(void);
static inline void rto_timer_destroy(void);
static void rto_timer_start(void);
static void rto_timer_stop(void);

static unsigned char note_global_id;

int communication_init(void) {
    return rto_timer_init();
}

void communication_cleanup(void) {
    rto_timer_destroy();
}

int communication_connect(serial_context_t *ctx) {
    note_global_id = 0;
    return (communication_craft_and_send(ctx, NOTE_TYPE_HND, NULL, 0) != 0) ? 1 : 0;
}

static unsigned char _receive_attempt(serial_context_t *ctx, note_t *n) {
    static const struct timespec poll_tm = {0, ONE_MSEC * 2};
    unsigned char *n_raw = (unsigned char*) n;
    unsigned char id, size = 0, received = 0;
    while(1) {
        if(rto_timer.elapsed) return E_TIMEOUT_ELAPSED;
        if(!serial_read_getChar(ctx, n_raw + received)) nanosleep(&poll_tm, NULL);
        else switch(++received) {
            case 1:
                id = note_get_id(n);
                if(id != note_global_id) return E_ID_MISMATCH;
                break;
            case 2:
                if(note_check_header(n) != 0) return E_CORRUPTED_HEADER;
                size = note_get_size(n);
                break;
            default:
                if(received >= size)
                    return (note_check_crc(n) != 0) ? E_CORRUPTED_CRC : E_SUCCESS;
                break;
        }
    }
}

int communication_send(_serial_context_t *ctx, const note_t *n) {
    uint8_t size = note_get_size(n);
    if(!ctx || !p || !size || size > sizeof(note_t)) return 1;
    serial_read_flush();
    for(uint8_t attempt = 0; attempt < MAX_SEND_ATTEMPTS; ++attempt) {
        rto_timer_start();
        serial_send(ctx, n, size);
        note_t response[1];
        uint8_t ret = _receive_attempt(ctx, response);
        if(ret == E_SUCCESS && note_get_type == NOTE_TYPE_ACK) {
            rto_timer_stop();
            note_global_id = note_next_id(note_global_id);
            return 0;
        }
        else if(ret != E_TIMEOUT_ELAPSED) 
            while(!rto_timer.elapsed)
                pause();
    }
    note_global_id = 0;
    return 1;
}

int communication_receive(serial_context_t *ctx, note_t *n) {
    if(!ctx || !n) return 1;
    note_t response[1];
    for(unsigned char attempt = 0; attempt < MAX_RECV_ATTEMPTS; ++attempt) {
        rto_timer_start();
        unsigned char ret = _receive_attempt(ctx, n);
        switch(ret) {
            case E_SUCCESS:
                note_ack(n, response);
                serial_send(ctx, response, NOTE_MIN_SIZE);
                rto_timer_stop();
                note_global_id = note_next_id(note_global_id);
                return 0;
            case E_TIMEOUT_ELAPSED:
                break;
            case E_ID_MISMATCH:
            case E_CORRUPTED_HEADER:
            case E_CORRUPTED_CRC:
                note_err(n, response);
                serial_send(ctx, response, NOTE_MIN_SIZE);
                while(!rto_timer.elapsed)
                    pause();
                break;
            default: break;
        }
    }
    note_global_id = 0;
    return 1;
}

int communication_craft_and_send(serial_context_t *ctx, unsigned char type, const unsigned char *data, unsigned char data_size) {
    note_t n[1];
    id(note_craft(note_global_id, type, data, data_size, n) != 0) return 1;
    return communication_send(ctx, n);
}

int communication_cmd(serial_context_t *ctx, command_id_t cmd) {
    if(!ctx || cmd >= COMMAND_COUNT) return 1;
    unsigned char _payload[NOTE_DATA_MAX_SIZE];
    command_payload_t *payload = (command_payload_t*) _payload;
    payload->id = cmd;
    return communication_craft_and_send(ctx, NOTE_TYPE_CMD, _payload, sizeof(command_id_t));
}

static void rto_sig_handler(int sig) {
    rto_timer.elapsed = 1;
    rto_timer.active = 0;
}

static inline int rto_timer_init(void) {
    rto_timer = (timer_entity_t) {
        .timeval = {
            {0, 0},
            {0, RTO_VALUE_MSEC * ONE_MSEC}
        },
        .sig_ev = {SIGEV_SIGNAL, SIGUSR1},
        .sig_act = (struct sigaction) {.sa_handler = rto_sig_handler }
    };
    if(timer_create(CLOCK_REALTIME, &rto_timer.sig_ev, &rto_timer.id) != 0) {
        //ERROR
    }
    if(sigaction(SIGUSR1, &rto_timer.sig_act, &rto_timer.sig_act_old) != 0) {
        timer_delete(rto_timer.id);
        //ERROR
    }
    return 0;
}

static inline void rto_timer_destroy(void) {
    if(rto_timer.active)
        rto_timer_stop();
    if(sigaction(SIGUSR1, &rto_timer.sig_act_old, NULL) != 0) {
        //ERROR
    }
    timer_delete(rto_timer.id);
}

static void rto_timer_start(void) {
    if(rto_timer.active) rto_timer_stop();
    rto_timer.elapsed = 0;
    timer_settime(rto_timer.id, 0, &rto_timer.timeval, NULL);
}

static void rto_timer_stop(void) {
    static const struct itimerspec disarmer = { 0 };
    if(!rto_timer.active) return;
    timer_settime(rto_timer.id, 0, &disarmer, NULL);
    rto_timer.active = 0;
}