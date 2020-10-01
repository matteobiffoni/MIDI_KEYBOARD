#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <unistd.h>
#include <math.h>

#define SAMPLE_RATE (22050)
#define BIT_REPRESENTATION (4095)
#define MY_PI (3.14159265359)

#define NOTES (7)
#define BUFFERS (2)

ALCdevice *device;
ALCcontext *context;

static ALuint buffers[NOTES][BUFFERS], sources[NOTES];
static pthread_t note_handler_threads[NOTES];

static unsigned quit = 0, start = 0;

static unsigned BUF_SIZES[] = {
    3034,
    2854,
    2140,
    2148,
    2700,
    3106,
    2500
};

static unsigned flags[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static float note_frequencies_octave_4[] = {
    261.63f,
    293.66f,
    329.63f,
    349.23f,
    392.f,
    440.f,
    493.88f
};

short* generateWave(int index) {
    short* result = malloc(sizeof(short) * BUF_SIZES[index]);
    for(int i = 0; i < BUF_SIZES[index]; i++) {
        result[i] = 0;
        if(flags[index])
            result[i] = BIT_REPRESENTATION * sin((2.f * MY_PI * note_frequencies_octave_4[index]) * i / SAMPLE_RATE);
    }
    return result;
}

void *note_handler(void* args) {
    int index = *((int*) args);
    short* samples = generateWave(index);
    for(int i = 0; i < BUFFERS; i++) {
        alBufferData(buffers[index][i], AL_FORMAT_MONO16, samples, BUF_SIZES[index], SAMPLE_RATE);
        alSourceQueueBuffers(sources[index], 1, &buffers[index][i]);
    }
    alSourcePlay(sources[index]);
    start = 1;;
    int processed = 0;
    int totalProcessed = 0;
    while(!quit) {
        samples = generateWave(index);
        usleep(10 * 1000);
        ALuint uiProcessed = 0;
        alGetSourcei(sources[index], AL_BUFFERS_PROCESSED, &processed);
        totalProcessed += processed;
        while(processed) {
            uiProcessed = 0;
            alSourceUnqueueBuffers(sources[index], 1, &uiProcessed);
            alBufferData(uiProcessed, AL_FORMAT_MONO16, samples, BUF_SIZES[index], SAMPLE_RATE);
            alSourceQueueBuffers(sources[index], 1, &uiProcessed);
            processed--;
        }
    }
    free(samples);
    alSourceStop(sources[index]);
}

void al_stuff(void) {
    const char *defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    device = alcOpenDevice(defname);
    if(device == NULL) {
        fprintf(stderr, "Error opening default device\n");
        return;
    }
    context = alcCreateContext(device, NULL);
    if(context == NULL) {
        fprintf(stderr, "Error creating context\n");
        return;
    }
    if(!alcMakeContextCurrent(context)) {
        fprintf(stderr, "Error making current context\n");
        alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }
    alGenSources((ALuint) NOTES, sources);
    for(int i = 0; i < NOTES; i++) {
        alGenBuffers((ALuint) BUFFERS, buffers[i]);
        int *arg = malloc(sizeof(*arg));
        if(arg == NULL) {
            fprintf(stderr, "Could not allocate memory for note handler thread [%d]\n", i);
            return;
        }
        *arg = i;
        int rc = pthread_create(&note_handler_threads[i], NULL, note_handler, arg);
        if(rc) {
            fprintf(stderr, "Error creating note handler thread [%d]\n", i);
            return;
        }
    }
    for(int i = 0; i < NOTES; i++) {
        int rc = pthread_join(note_handler_threads[i], NULL);
        if(rc) {
            fprintf(stderr, "Error waiting for thread [%d] to terminate\n", i);
            return;
        }
    }
    alSourceStopv((ALuint) NOTES, sources);
    alDeleteSources((ALuint) NOTES, sources);
    for(int i = 0; i < NOTES; i++) {
        alDeleteBuffers((ALuint) BUFFERS, buffers[i]);
    }
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

int translater(char c) {
    switch(c) {
        case 'c':
            return 0;
        case 'd':
            return 1;
        case 'e':
            return 2;
        case 'f':
            return 3;
        case 'g':
            return 4;
        case 'a':
            return 5;
        case 'b':
            return 6;
        default:
            return -1;
    }
}

void printFlags() {
    printf("-------FLAGS-------\n");
    printf(" C: %d\n", flags[0]);
    printf(" D: %d\n", flags[1]);
    printf(" E: %d\n", flags[2]);
    printf(" F: %d\n", flags[3]);
    printf(" G: %d\n", flags[4]);
    printf(" A: %d\n", flags[5]);
    printf(" B: %d\n", flags[6]);
}

void *key_listener(void* args) {
    while(!start) {
        usleep(10 * 1000);
    }
    while(!quit) {
        char c = getchar();
        if(c == '\n') continue;
        int translated = translater(c);
        if(translated == -1) {
            quit = 1;
            continue;
        }
        flags[translated] = !flags[translated];
        printFlags();
    }
}

int main(void) {
    pthread_t key_listener_thread;
    int rc = pthread_create(&key_listener_thread, NULL, key_listener, NULL);
    if(rc) {
        fprintf(stderr, "Error creating key listener thread\n");
        return 1;
    }
    al_stuff();
    pthread_join(key_listener_thread, NULL);
    return 0;
}