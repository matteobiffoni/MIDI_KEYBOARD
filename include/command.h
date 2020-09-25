// MIDI KEYBOARD -- Matteo Biffoni
// Command Module
#ifndef __COMMAND_MODULE_H
#define __COMMAND_MODULE_H

#include <stdint.h>

#define COMMAND COUNT 2
typedef enum COMMAND_ID_E {
    CMD_START,
    CMD_STOP
} command_id_t;

typedef enum COMMAND_RETVAL_E {
    CMD_RET_FINISHED,
    CMD_RET_ACTIVE,
    CMD_RET_ERROR,
    CMD_RET_NOT_EXISTS,
    CMD_RET_NOT_IMPLEMENTED
} cmd_retval_t;

typedef struct _command_payload_s {
    uint8_t id;
} command_payload_t;

#ifdef AVR
#include "arduino/communication.h"
typedef uint8_t (*command_action_f) (const void *arg);

typedef struct _command_s {
    command_action_f execute;
    command_action_f iterate;
    communication_opmode_t opmode;
} command_t;

void command_init(void);

uint8_t command_execute(command_id_t id);

uint8_t command_iterate(command_id_t id);

#endif // AVR

#endif // __COMMAND_MODULE_H