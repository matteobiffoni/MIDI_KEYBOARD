// MIDI KEYBOARD -- Matteo Biffoni
// Note header file
#ifndef __NOTE_MODULE_H
#define __NOTE_MODULE_H

 #define NOTE_DATA_MAX_SIZE 7
 #define NOTE_MIN_SIZE 2
 #define NOTE_MAX_SIZE sizeof(note_t)

// Note types
typedef enum NOTE_TYPE_E {
    // TODO: REVIEW
    NOTE_TYPE_HND = 0x00, // Handshake
    NOTE_TYPE_ACK = 0x01, // Acknowledgment
    NOTE_TYPE_ERR = 0x02, // Error
    NOTE_TYPE_DAT = 0x03  // Data
} note_type_t;

// Note definition
typedef struct _note_s {
    uint8_t header;
        // unsigned type       : 2;
        // unsigned size       : 5;
        // unsigned header_par : 1;
    uint8_t data[NOTE_DATA_MAX_SIZE + sizeof(crc_t)];
} note_t;

// Craft a note (preallocated as 'dest'): 0 if correct parameters, 1 otherwise
uint8_t note_craft(note_type_t type, const uint8_t *data, uint8_t data_size, note_t *dest);

// Check parity bit of header
uint8_t note_check_header(const note_t*);

// Compute the header parity bit: returns the bit computed
uint8_t note_header_parity(const note_t*);

// Check the note's CRC
uint8_t note_check_crc(const note_t*);

// Craft an aknowledgment note
uint8_t note_ack(const note_t*, note_t *dest);

// Craft an error note
uint8_t note_err(const note_t*, note_t *dest);

// 1 if note brings data, 0 otherwise
uint8_t note_brings_data(const note_t*);

// Getters
uint8_t note_get_header_par(const note_t*);
uint8_t note_get_type(const note_t*);
uint8_t note_get_size(const note_t*);

// Data size
uint8_t note_data_size(const note_t*);

#endif // __NOTE_MODULE_H
