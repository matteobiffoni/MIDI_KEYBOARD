// MIDI KEYBOARD -- Matteo Biffoni
// CRC Algorithm
#ifndef __CRC_MODULE_H
#define __CRC_MODULE_H
#include <stdint.h>

// Parameters
#define CRC_NAME "CRC-8"
#define CRC_POLY 0x07
#define CRC_INIT 0x00
#define CRC_CHECK 0xF4

// Definition
typedef uint8_t crc_t;

// Get the CRC from data (size does not count the room for CRC)
crc_t crc(const void *data, uint8_t size);

// CRC check
crc_t crc_check(const void *data, uint8_t size);

#endif // __CRC_MODULE_H