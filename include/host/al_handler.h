// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// OpenAL handler definition
// Header file

#include <AL/al.h>      // Needed for openal functions
#include <AL/alc.h>     // Needed for openal functions
#include <unistd.h>     // Needed for ^usleep()^
#include <pthread.h>    // Needed for threads
#include <stdlib.h>     // Needed for ^malloc()^
#include <math.h>       // Needed for ^sin()^

#include "note_event.h" // Needed to know how to update flags

// Notes number
#define NOTES (7)

// Buffers number for openal queues
#define BUFFERS (2)

// Sample rate definition
#define SAMPLE_RATE(FREQ) (FREQ * 50)
// Buffer size definition for audio buffers
#define BUF_SIZE_MIN (3200)
// Definition of bit to use for openal
#define BIT_REPRESENTATION (4095)
// PI
#define MY_PI (3.14159265359)

// Quit state flag enumeration
enum QUIT_STATE_E {
    NOT_QUIT = 0,                       // Program still executing
    QUIT_OK,                            // Program stopped normally
    QUIT_PARITY_ERROR,                  // Parity bit not correct
    QUIT_FLAGS_ERROR,                   // Flags were updated in the wrong way
    QUIT_DEVICE_OPEN_FAILURE,           // Could not open device
    QUIT_AUDIO_CONTEXT_CREATION_ERROR,  // Could not create audio context
    QUIT_AUDIO_CONTEXT_SETTING_ERROR,   // Could not set audio context
    QUIT_MEMORY_ALLOCATOR_ERROR,        // Could not execute malloc()
    QUIT_THREAD_CREATION_ERROR,         // Could not create a thread
    QUIT_THREAD_JOINING_ERROR,          // Join thread error
    QUIT_UNKNOWN_ERROR                  // Unknown error
};

// Quit state flag
extern int quit_state;

// The audio device
ALCdevice *device;

// The audio context
ALCcontext *context;

// Buffers used for openal queues
ALuint buffers[NOTES][BUFFERS], sources[NOTES];

// Threads used to handle each note
pthread_t note_handler_threads[NOTES];

// Flags to control the notes flow
extern unsigned flags[BUFFERS];

// Definition of constant notes frequencies
const float note_frequencies_octave_4[NOTES];

// Update the flags
int update_flags(note_event_t event);

// Generate a buffer of shorts containing the audio data
short* generate_wave(int index);

// Note handler routine
void *note_handler(void* args);

// Openal routine
void *al_handler(void* args);