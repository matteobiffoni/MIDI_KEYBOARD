// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Communication protocol constraints and method definition
// Header file

#include <termios.h>    // Needed for handling avr communication
#include <fcntl.h>      // Needed for attributes of avr file descriptor
#include <stdio.h>      // Needed for I/O
#include <sys/types.h>  // Needed for attribute types
#include <sys/stat.h>   // ""                       ""
#include <pthread.h>    // Needed for handling multithreading
#include <unistd.h>     // Needed for ^isatty()^

#include <stdint.h>     // Needed for <uint8_t>


#include "al_handler.h" // Needed for ^updateFlags()^

// Device name definition
#define AVR_DEVICE ("/dev/ttyACM0")

// Baud rate definition
#define BAUD_RATE (B115200)

// AVR file descriptor
int avr_device_fd;

// The avr device
struct termios avr_device;

// Initialization and setup of the communication with AVR
uint8_t communication_init_and_setup(void);

// Close the communication with AVR
void communication_close(void);

// Function (blocking) that reads the bytes incoming from the AVR
void start_reading(void);
