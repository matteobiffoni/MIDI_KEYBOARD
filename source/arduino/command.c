// MIDI KEYBOARD -- Matteo Biffoni
// Command implementation
#include "../../include/command.h"
#include "../../include/arduino/communication.h"

static command_t *cmd_table[COMMAND_COUNT];

extern command_t *cmd_start;
extern command_t *cmd_stop;

uint8_t command_execute(command_id_t id) {
    if(id >= COMMAND_COUNT) return CMD_RET_NOT_EXISTS;
    uint8_t has_opmode = cmd_table[id]->opmode != NULL ? 1 : 0;
    if(has_opmode) communication_opmode_switch(cmd_table[id]->opmode);
    if(!cmd_table[id]->start) return has_opmode ? CMD_RET_ACTIVE : CMD_RET_FINISHED;
    return cmd_table[id]->execute();
}

uint8_t command_iterate(command_id_t id) {
    if(id >= COMMAND_COUNT) return CMD_RET_NOT_EXISTS;
    if(!cmd_table[id]->iterate) return CMD_RET_NOT_IMPLEMENTED;
    return cmd_table[id]->iterate();
}

void command_init(void) {
    cmd_table[CMD_START] = cmd_start;
    cmd_table[CMD_STOP] = cmd_stop;
}