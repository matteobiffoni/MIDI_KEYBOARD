// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// OpenAL handler implementation
// Source file

#include "al_handler.h"     // Header file

int quit_state = NOT_QUIT;  // Quit state flag

// Flags to control the notes flow
unsigned flags[BUFFERS];

short empty_samples[BUF_SIZE], filled_samples[BUF_SIZE];

// Definition of constant notes frequencies
const float note_frequencies_octave_4[NOTES] = {
    261.63f,
    293.66f,
    329.63f,
    349.23f,
    392.f,
    440.f,
    493.88f
};

// Prints the status of notes
static inline void print_flags() {
    printf("-------FLAGS-------\n");
    printf(" C: %d\n", flags[0]);
    printf(" D: %d\n", flags[1]);
    printf(" E: %d\n", flags[2]);
    printf(" F: %d\n", flags[3]);
    printf(" G: %d\n", flags[4]);
    printf(" A: %d\n", flags[5]);
    printf(" B: %d\n", flags[6]);
}

// Update the flags
int update_flags(note_event_t event) {
    uint8_t state = event.state;
    uint8_t value = event.value;
    if(!value || value > NOTES) return 1;
    if(state == flags[value - 1]) return 1;
    flags[value - 1] = state;
    return 0;
}

// Generate a buffer of shorts containing the audio data
void generate_wave(short* samples, char empty) {
    for(int i = 0; i < BUF_SIZE; i++) {
        if(empty) samples[i] = 0;
        else {
            float sin_value = sin(2.f * MY_PI * i / 50); 
            samples[i] = BIT_REPRESENTATION * sin_value;
        }
    }
}

// Openal routine
void* al_handler(void* args) {
    const char *default_device_name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    device = alcOpenDevice(default_device_name);
    if(device == NULL) {
        quit_state = QUIT_DEVICE_OPEN_FAILURE;
        return NULL;
    }
    context = alcCreateContext(device, NULL);
    if(context == NULL) {
        quit_state = QUIT_AUDIO_CONTEXT_CREATION_ERROR;
        return NULL;
    }
    if(!alcMakeContextCurrent(context)) {
        alcDestroyContext(context);
        alcCloseDevice(device);
        quit_state = QUIT_AUDIO_CONTEXT_SETTING_ERROR;
        return NULL;
    }
    alGenSources((ALuint) NOTES, sources);
    for(int i = 0; i < NOTES; i++) {
        alGenBuffers((ALuint) BUFFERS, buffers[i]);
    }
    generate_wave(empty_samples, 1);
    generate_wave(filled_samples, 0);
    for(int i = 0; i < NOTES; i++) {
        for(int j = 0; j < BUFFERS; j++) {
            alBufferData(buffers[i][j], AL_FORMAT_MONO16, empty_samples, BUF_SIZE * sizeof(short), SAMPLE_RATE(note_frequencies_octave_4[i]));
        }
        alSourceQueueBuffers(sources[i], BUFFERS, buffers[i]);
        alSourcePlay(sources[i]);
    }
    int processed = 0;
    ALuint ui_processed = 0;
    while(!quit_state) {
        for(int i = 0; i < NOTES; i++) {
            alGetSourcei(sources[i], AL_BUFFERS_PROCESSED, &processed);
            while(processed) {
                alSourceUnqueueBuffers(sources[i], 1, &ui_processed);
                if(flags[i]) {
                    alBufferData(ui_processed, AL_FORMAT_MONO16, filled_samples, BUF_SIZE * sizeof(short), SAMPLE_RATE(note_frequencies_octave_4[i]));
                }
                else {
                    alBufferData(ui_processed, AL_FORMAT_MONO16, empty_samples, BUF_SIZE * sizeof(short), SAMPLE_RATE(note_frequencies_octave_4[i]));
                }
                alSourceQueueBuffers(sources[i], 1, &ui_processed);
                processed--;
            }
        }
    }
    alDeleteBuffers((ALuint) 1, &ui_processed);
    alSourceStopv((ALuint) NOTES, sources);
    alDeleteSources((ALuint) NOTES, sources);
    for(int k = 0; k < NOTES; k++) {
        alDeleteBuffers((ALuint) BUFFERS, buffers[k]);
    }
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    return NULL;
}