#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <stdint.h>
#include <unistd.h> // for usleep()
#include <pthread.h>

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)


typedef struct
{
    uint32_t total_count;
    uint32_t up_count;

    uint32_t counter;
    uint32_t prev_freq;
    uint32_t freq;
} paTestData;

//volatile int freq = 0;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    uint8_t *out = (uint8_t*)outputBuffer;
    unsigned long i;
    uint32_t freq = data->freq;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for( i=0; i<framesPerBuffer; i++ )
    {
        if(data->up_count > 0 && data->total_count == data->up_count) {
            *out++ = 0x00;
            continue;
        }
        data->total_count++;

        if(freq != data->prev_freq) {
            data->counter = 0;
        }

        if(freq) {
            int overflow_max = SAMPLE_RATE / freq;
            uint32_t data_cnt = data->counter % overflow_max;
            if(data_cnt > overflow_max/2)
                *out++ = 0xff;
            else {
                *out++ = 0x00;
            }
            data->counter++;
        }
        else {
            data->counter = 0;
            *out++ = 0;
        }
        data->prev_freq = freq;
    }

    return paContinue;
}

static PaStream *stream;
static paTestData data;


void buzzer_set_freq(int frequency)
{
    data.up_count = 0; // do not stop!
    data.freq = frequency;
}

void buzzer_beep(int frequency, int msecs)
{
    data.total_count = 0;
    data.up_count = SAMPLE_RATE * msecs / 1000;
    data.freq = frequency;
}

int buzzer_start(void)
{
    PaStreamParameters outputParameters;

    PaError err;
    int i;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = 1;       /* stereo output */
    outputParameters.sampleFormat = paUInt8; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
        patestCallback,
        &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;

}

int buzzer_stop() 
{
    PaError err = 0;
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();

    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
void msleep(int d){
    usleep(d*1000);
}
typedef enum {
    NONE = 0x00,
    C,
    D,
    E,
    F,
    G,
    A,
    B
} CURRENT_NOTE;
static CURRENT_NOTE current_note = NONE;
static int quit = 0;

void *key_listener(void * arg) {
    char input;
    while(1) {
        scanf("%c", &input);
        switch(input) {
            case '\n':
                break;
            case 'a':
                if(current_note == A) current_note = NONE;
                else current_note = A;
                break;
            case 'b':
                if(current_note == B) current_note = NONE;
                else current_note = B;
                break;
            case 'c':
                if(current_note == C) current_note = NONE;
                else current_note = C;
                break;
            case 'd':
                if(current_note == D) current_note = NONE;
                else current_note = D;
                break;
            case 'e':
                if(current_note == E) current_note = NONE;
                else current_note = E;
                break;
            case 'f':
                if(current_note == F) current_note = NONE;
                else current_note = F;
                break;
            case 'g':
                if(current_note == G) current_note = NONE;
                else current_note = G;
                break;
            case 'q':
                current_note = NONE;
                quit = 1;
                pthread_exit(0);
            default:
                current_note = NONE;
                break;
        }
    }
}
int getFreqFromNote() {
    switch (current_note) {
    case C: return 261;
    case D: return 293;
    case E: return 329;
    case F: return 349;
    case G: return 392;
    case A: return 440;
    case B: return 494;
    default:return 0;
    }
}
char current_note_to_string() {
    switch (current_note)
    {
        case C:
            return 'C';
        case D:
            return 'D';
        case E:
            return 'E';
        case F:
            return 'F';
        case G:
            return 'G';
        case A:
            return 'A';
        case B:
            return 'B';
        default:
            return '/';
    }
}
void *current_note_listener(void* arg) {
    CURRENT_NOTE previous_current_note = NONE;
    buzzer_start();
    while(!quit) {
        if(current_note != previous_current_note) {
            previous_current_note = current_note;
            printf("Current note: %c\n", current_note_to_string());
            buzzer_set_freq(getFreqFromNote());
        }
    }
}

int main(void) {

    // notes frequency chart: http://www.phy.mtu.edu/~suits/notefreqs.html
    
    pthread_t key_listener_thread, current_note_listener_thread;
    int rc = pthread_create(&key_listener_thread, NULL, key_listener, (void*) 0);
    if(rc) {
        printf("Error creating key listener thread\n");
        return 1;
    }
    rc = pthread_create(&current_note_listener_thread, NULL, current_note_listener, (void*) 0);
    if(rc) {
        printf("Error creating current note listener thread\n");
        return 1;
    }
    pthread_join(key_listener_thread, NULL);
    pthread_join(current_note_listener_thread, NULL);
    return 0;
}
