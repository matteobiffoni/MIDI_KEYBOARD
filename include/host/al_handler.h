// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// OpenAL handler definition
// Header file

#include <AL/al.h>
#include <AL/alc.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

#include "note_event.h"

#define NOTES (7)
#define BUFFERS (2)

#define SAMPLE_RATE(FREQ) (FREQ * 50)
#define BUF_SIZE_MIN (3200)
#define BIT_REPRESENTATION (4095)
#define MY_PI (3.14159265359)

ALCdevice *device;

ALCcontext *context;

ALuint buffers[NOTES][BUFFERS], sources[NOTES];

pthread_t note_handler_threads[NOTES];

extern unsigned flags[BUFFERS];

const float note_frequencies_octave_4[NOTES];

int updateFlags(note_event_t event);

short* generateWave(int index);

void *note_handler(void* args);

void *al_handler(void* args);