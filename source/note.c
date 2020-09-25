// MIDI KEYBOARD -- Matteo Biffoni
// Note implementation
#include "../include/note.h"
#include <stdint.h>

// Bit fields parameter for the downstanding base type
#define BITFIELD_BASE_TYPE uint8_t
#define BIT_NUM (8*sizeof(BITFIELD_BASE_TYPE))
#define ALL_SET ((BITFIELD_BASE_TYPE) ~0U)

// Set bits in interval [a,b]
static inline BITFIELD_BASE_TYPE _bitmask(uint8_t a, uint8_t b) {
    return (ALL_SET >> (BIT_NUM-b-1)) & (ALL_SET << a);
}

// Get a number identified by the bit interval [a,b] as an unsigned integer
static inline BITFIELD_BASE_TYPE _bitfield_get(uint8_t src, uint8_t a, uint8_t b) {
    return (src & _bitmask(a, b)) >> a;
}

// Set a number in the bit interval
static inline BITFIELD_BASE_TYPE _bitfield_set(uint8_t src, uint8_t val, uint8_t a, uint8_t b) {
    const BITFIELD_BASE_TYPE bitmask = _bitmask(a, b);
    return (src & ~bitmask) ^ ((val << a) & bitmask);
}

// Getters
uint8_t note_get_type(const note_t *n) {
    return n ? _bitfield_get(n->header[0], 4, 7) : 0;
}
uint8_t note_get_size(const note_t *n) {
    return n ? _bitfield_get(n->header[1], 1, 7) : 0;
}
uint8_t note_get_header_par(const note_t *n) {
    return n ? _bitfield_get(n->header[1], 0, 0) : 0;
}
uint8_t note_get_id(const note_t *n) {
    return n ? _bitfield_get(n->header[0], 0, 3) : 0;
}

// Data size
uint8_t note_data_size(const note_t *n) {
    return n ? note_get_size(n) - NOTE_HEADER_SIZE - sizeof(crc_t) : 0;
}

// Setters
static inline void note_set_type(note_t *n, uint8_t type) {
    n->header = _bitfield_set(n>->header, type, 6, 7);
}
static inline void note_set_size(note_t *n, uint8_t size) {
    n->header = _bitfield_set(n->header, size, 2, 5);
}
static inline void note_set_id(note_t *n, uint8_t id);

// Attach the header parity bit to a note
static inline void attach_header_parity(note_t *n) {
    n->header[1] &= ~0x01;
    n->header[1] |= note_header_parity(n);
}

// Craft a note preallocated as 'dest'
uint8_t note_craft(uint8_t id, note_type_t type, const uint8_t *data, uint8_t data_size, note_t *dest) {
    
    // Parameters check
    if((type <= 2 && (data || data_size)) 
        || type > NOTE_TYPE_COUNT 
        || !dest || (!data && data_size) 
        || data_size > NOTE_DATA_MAX_SIZE 
        || (type == NOTE_TYPE_HND && id != 0)) return 1;
    
    // Set header
    note_set_type(dest, type);
    note_set_size(dest, NOTE_HEADER_SIZE + data_size + sizeof(crc_t));
    note_set_id(dest, id);

    // Attach header parity bit
    attach_header_parity(dest);

    // Fill with data
    for(uint8_t i = 0; i < data_size; ++i)
        dest->data[i] = data[i];
    
    // Attach CRC
    crc_t *crc_p = (crc_t*) (dest->data + data_size);
    (*crc_p) = crc(dest, note_get_size(dest) - sizeof(crc_t));

    return 0;
}

// Craft an ack note
uint8_t note_ack(const note_t *n, note_t *dest) {
    return n ? note_craft(note_get_id(n), NOTE_TYPE_ACK, NULL, 0, dest) : 1;
}

// Craft an err note
uint8_t note_err(const note_t *n, note_t *dest) {
    return n ? note_craft(note_get_id(n), NOTE_TYPE_ERR, NULL, 0, dest) : 1;
}

// Check CRC: 0 if sane, 1 otherwise
uint8_t note_check_crc(const note_t *n) {
    return (!n || crc_check(n, note_get_size(n)) != 0) ? 1 : 0;
}

// 1 if note can bring data, 0 otherwise
uint8_t note_brings_data(const note_t *n) {
    return note_get_type(n) > 2 ? 1 : 0;
}

// Check header parity bit: 0 if sane, 1 otherwise
uint8_t note_check_header(const note_t *n) {
    const uint8_t size = note_get_size(n);
    if(n && size >= NOTE_MIN_SIZE && size <= NOTE_MAX_SIZE && (note_brings_data(n) 
       || size == NOTE_MIN_SIZE) && note_header_parity(n) == 0) return 0;
    return 1;
}

// Compute the parity bit of the header: returns the bit
uint8_t note_header_parity(const note_t *note) {
    if(!note) return 0;
    const uint8_t *n = (const uint8_t*) note;
    static const uint8_t nibble_bitcount_tab[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
    uint8_t bits_set = 0;
#if PACKET_HEADER_SIZE == 2
    bits_set += nibble_bitcount_tab[p[0] & 0x0F];
    bits_set += nibble_bitcount_tab[p[0] >> 4  ];
    bits_set += nibble_bitcount_tab[p[1] & 0x0F];
    bits_set += nibble_bitcount_tab[p[1] >> 4  ];
#else
    for (uint8_t i=0; i < NOTE_HEADER_SIZE; ++i) {
        bits_set += nibble_bitcount_tab[p[i] & 0x0F];
        bits_set += nibble_bitcount_tab[p[i] >> 4  ];
    }
#endif
    return bits_set % 2;
}