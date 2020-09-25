// MIDI KEYBOARD -- Matteo Biffoni
// Stop command implementation
#include <stddef.h>
#include "../../../include/command.h"

#define COMMAND_NAME cmd_stop

static uint8_t _execute(void) {
    return CMD_RET_FINISHED;
}

static command_t _cmd = {
    .execute = _execute,
    .iterate = NULL,
    .opmode = NULL
};

command_t *COMMAND_NAME = &_cmd;