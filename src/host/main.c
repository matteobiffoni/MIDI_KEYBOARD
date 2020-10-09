// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// Host main program
// Source file

#include <pthread.h>        // Needed for handling multithreading
#include <stdio.h>          // Needed for I/O

#include "communication.h"  // Needed for communication with AVR

// Array of closing messages
const char* closing_msg[] = {
    "UNKNOWN",
    "Everything ok: communication closed and audio device closed. Bye!",
    "Parity check not accepted",
    "Something went wrong with notes flags",
    "Error opening default audio device",
    "Error creating audio context",
    "Error setting current audio context",
    "Could not allocate memory for note handler thread",
    "Error creating note handler thread",
    "Error waiting for note handler thread to terminate",
    "Something unknown went wrong"
};

// Quit listener routine
void* quit_listener(void* args) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    fprintf(stdout, "Press enter to stop\n");
    getchar();
    quit_state = QUIT_OK;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    return NULL;
}

// Host main program
int main(void) {
    pthread_t al_thread, quit_listener_thread;
    if(!communication_init_and_setup()) {
        pthread_create(&al_thread, NULL, al_handler, NULL);
        pthread_create(&quit_listener_thread, NULL, quit_listener, NULL);
        start_reading();
        communication_close();
        pthread_join(al_thread, NULL);
        if(quit_state == NOT_QUIT) {
            quit_state = QUIT_UNKNOWN_ERROR;
        }
        FILE* ch;
        int ret = 0;
        if(quit_state == QUIT_OK)
            ch = stdout;
        else {
            ch = stderr;
            ret = quit_state;
            pthread_cancel(quit_listener_thread);
        }
        fprintf(ch, "%s\n", closing_msg[quit_state]);
        pthread_join(quit_listener_thread, NULL);
        return ret;
    }
    return 1;
}