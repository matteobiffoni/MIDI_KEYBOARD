// Project: MIDI KEYBOARD
// Author:  Matteo Biffoni

// OpenAL handler implementation
// Source file

#include "al_handler.h"

unsigned flags[BUFFERS];
const float note_frequencies_octave_4[NOTES] = {
    261.63f,
    293.66f,
    329.63f,
    349.23f,
    392.f,
    440.f,
    493.88f
};

static inline int calc_buf_size(int index) {
    return BUF_SIZE_MIN + (200 * (index + 1));
}

static inline void printFlags() {
    printf("-------FLAGS-------\n");
    printf(" C: %d\n", flags[0]);
    printf(" D: %d\n", flags[1]);
    printf(" E: %d\n", flags[2]);
    printf(" F: %d\n", flags[3]);
    printf(" G: %d\n", flags[4]);
    printf(" A: %d\n", flags[5]);
    printf(" B: %d\n", flags[6]);
}

int updateFlags(note_event_t event) {
    uint8_t state = event.state;
    uint8_t value = event.value;
    if(!value || value > NOTES) return 1;
    if(state == flags[value - 1]) return 1;
    flags[value - 1] = state;
    return 0;
}

short* generateWave(int index) {
    int buf_size = calc_buf_size(index);
    short* result = malloc(sizeof(short) * buf_size);
    for(int i = 0; i < buf_size; i++) {
        result[i] = 0;
        if(flags[index]) {
            float sin_value = sin((2.f * MY_PI * note_frequencies_octave_4[index]) * i / SAMPLE_RATE(note_frequencies_octave_4[index])); 
            result[i] = BIT_REPRESENTATION * sin_value;
        }
    }
    return result;
}

void* note_handler(void* args) {
    int index = *((int*) args);
    int buf_size = calc_buf_size(index);
    printf("BUF_SIZE for handler %d: %d\n", index, buf_size);
    short* samples = generateWave(index);
    for(int i = 0; i < BUFFERS; i++) {
        alBufferData(buffers[index][i], AL_FORMAT_MONO16, samples, buf_size / sizeof(short), SAMPLE_RATE(note_frequencies_octave_4[index]));
        alSourceQueueBuffers(sources[index], 1, &buffers[index][i]);
    }
    alSourcePlay(sources[index]);
    int processed = 0;
    int totalProcessed = 0;
    while(!quit_state) {
        usleep(10 * 1000);
        ALuint uiProcessed = 0;
        alGetSourcei(sources[index], AL_BUFFERS_PROCESSED, &processed);
        totalProcessed += processed;
        while(processed) {
            uiProcessed = 0;
            samples = generateWave(index);
            alSourceUnqueueBuffers(sources[index], 1, &uiProcessed);
            alBufferData(uiProcessed, AL_FORMAT_MONO16, samples, buf_size / sizeof(short), SAMPLE_RATE(note_frequencies_octave_4[index]));
            alSourceQueueBuffers(sources[index], 1, &uiProcessed);
            processed--;
        }
    }
    free(samples);
    alSourceStop(sources[index]);
    return NULL;
}

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
        int *arg = malloc(sizeof(*arg));
        if(arg == NULL) {
            quit_state = QUIT_MEMORY_ALLOCATOR_ERROR;
            return NULL;
        }
        *arg = i;
        int rc = pthread_create(&note_handler_threads[i], NULL, note_handler, arg);
        if(rc) {
            quit_state = QUIT_THREAD_CREATION_ERROR;
            return NULL;
        }
    }
    for(int j = 0; j < NOTES; j++) {
        int rc = pthread_join(note_handler_threads[j], NULL);
        if(rc) {
            quit_state = QUIT_THREAD_JOINING_ERROR;
            return NULL;
        }
    }
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