// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Communication protocol constraints and method definition
// Header file

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include <stdint.h>


#include "al_handler.h"

#define AVR_DEVICE ("/dev/ttyACM0")

#define BAUD_RATE (B115200)

int avr_device_fd;

struct termios avr_device;

uint8_t communication_init_and_setup(void);

void start_reading(void);
