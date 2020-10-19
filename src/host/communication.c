// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Communication protocol implementation
// Source file

#include "communication.h"      // Header file

// Initialization and setup of the communication with AVR
uint8_t communication_init_and_setup(void) {
    if(!AVR_DEVICE || *AVR_DEVICE == '\0') {
        fprintf(stderr, "AVR device name invalid\n");
        return 1;
    }
    avr_device_fd = open(AVR_DEVICE, O_RDONLY, O_NOCTTY);
    if(avr_device_fd < 0) {
        fprintf(stderr, "Error opening connection with %s\n", AVR_DEVICE);
        return 1;
    }
    if(!isatty(avr_device_fd)) {
        close(avr_device_fd);
        fprintf(stderr, "%s is not a TTY device\n", AVR_DEVICE);
        return 1;
    }
    fprintf(stdout, "Connected with %s\n", AVR_DEVICE);
    if(tcgetattr(avr_device_fd, &avr_device) != 0) {
        fprintf(stderr, "Error retreiving %s's attributes\n", AVR_DEVICE);
        close(avr_device_fd);
        return 1;
    }
    fprintf(stdout, "%s's attributes retrieved\n", AVR_DEVICE);
    if(cfsetispeed(&avr_device, BAUD_RATE) != 0) {
        fprintf(stderr, "Error setting baud rate\n");
        close(avr_device_fd);
        return 1;
    }
    fprintf(stdout, "Baud rate set\n");
    avr_device.c_cflag &= ~(CRTSCTS | PARENB | CSTOPB | CSIZE);
    avr_device.c_cflag |= (CS8| CREAD | CLOCAL);
    avr_device.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    avr_device.c_iflag &= ~(IXON | IXOFF | IXANY);
    avr_device.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    avr_device.c_iflag &= ~(INPCK);
    avr_device.c_cc[VMIN] = 0;
    avr_device.c_cc[VTIME] = 0;
    if(tcsetattr(avr_device_fd, TCSANOW, &avr_device) != 0) {
        fprintf(stderr, "Error setting attributes of %s\n", AVR_DEVICE);
        close(avr_device_fd);
        return 1;
    }
    fprintf(stdout, "Communication protocol initialization and setup done with %s\n", AVR_DEVICE);
    return 0;
}

// Non-blocking read: read continously a byte from the AVR
void *read_routine(void *args) {
    uint8_t byte_read = 0;
    while(!quit_state) {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        read(avr_device_fd, &byte_read, 1);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if(!byte_read) continue;
        note_event_t event = uint8_as_note_event(byte_read);
        if(check_parity_bit(event) != 0) {
            quit_state = QUIT_PARITY_ERROR;
        }
        if(update_flags(event)) {
            quit_state = QUIT_FLAGS_ERROR;
        }
        byte_read = 0;
    }
    return NULL;
}

// Close the communication with AVR
void communication_close(void) {
    if(avr_device_fd) {
        close(avr_device_fd);
        fprintf(stdout, "Communication protocol closed with %s\n", AVR_DEVICE);
    }
    return;
}

// Function (blocking) that reads the bytes incoming from the AVR
void start_reading(void) {
    pthread_t read_thread;
    pthread_create(&read_thread, NULL, read_routine, NULL);
    while(!quit_state) {
        usleep(100);
        if(quit_state) {
            pthread_cancel(read_thread);
        }
    }
    pthread_join(read_thread, NULL);
    return;
}