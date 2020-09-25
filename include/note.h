// MIDI KEYBOARD -- Matteo Biffoni
// Note header file
#ifndef __NOTE_MODULE_H
#define __NOTE_MODULE_H
#include <stdint.h>
#include "crc.h"

#define NOTE_ID_WIDTH_BIT 4
#define NOTE_ID_MAX_VAL (1 << NOTE_ID_WIDTH_BIT)
#define NOTE_DATA_MAX_SIZE 7
#define NOTE_MIN_SIZE 2
#define NOTE_MAX_SIZE sizeof(note_t)
#define NOTE_HEADER_SIZE 2

// Note types
#define NOTE_TYPE_COUNT 5
typedef enum NOTE_TYPE_E {
    // TODO: REVIEW
    NOTE_TYPE_HND = 0x00, // Handshake
    NOTE_TYPE_ACK = 0x01, // Acknowledgment
    NOTE_TYPE_ERR = 0x02, // Error
    NOTE_TYPE_CMD = 0x03,
    NOTE_TYPE_DAT = 0x04 // Data
} note_type_t;

// Note definition
typedef struct _note_s {
    uint8_t header[NOTE_HEADER_SIZE];
        // unsigned type       : 4;
        // unsigned id         : 4;
        // unsigned size       : 7;
        // unsigned header_par : 1;
    uint8_t data[NOTE_DATA_MAX_SIZE + sizeof(crc_t)];
} note_t;

// Craft a note (preallocated as 'dest'): 0 if correct parameters, 1 otherwise
uint8_t note_craft(uint8_t id, note_type_t type, const uint8_t *data, uint8_t data_size, note_t *dest);

// Compute the next or previous note ID
#define note_next_id(id) (((id) + 1) % NOTE_ID_MAX_VAL)
#define note_prev_id(id) (((id) + NOTE_ID_MAX_VAL - 1) % NOTE_ID_MAX_VAL)

// Check parity bit of header
uint8_t note_check_header(const note_t*);

// Compute the header parity bit: returns the bit computed
uint8_t note_header_parity(const note_t*);

// Check the note's CRC
uint8_t note_check_crc(const note_t*);

// Craft an aknowledgment note
uint8_t note_ack(const note_t*, note_t *dest);
#define note_ack_by_id(id, dest) note_craft(id, NOTE_TYPE_ACK, NULL, 0, dest)

// Craft an error note
uint8_t note_err(const note_t*, note_t *dest);
#define note_err_by_id(id, dest) note_craft(id, NOTE_TYPE_ERR, NULL, 0, dest)

// 1 if note brings data, 0 otherwise
uint8_t note_brings_data(const note_t*);

// Getters
uint8_t note_get_header_par(const note_t*);
uint8_t note_get_type(const note_t*);
uint8_t note_get_size(const note_t*);
uint8_t note_get_id(const note_t*);

// Data size
uint8_t note_data_size(const note_t*);

#endif // __NOTE_MODULE_H
