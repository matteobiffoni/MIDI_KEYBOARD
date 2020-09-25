// MIDI KEYBOARD -- Matteo Biffoni
// Serial implementation

#include <stdio.h>
#include <stdlib,h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include "../../include/client/serial.h"

#define ONE_MSEC 1000000
#define context_isValid(ctx)\
    ((ctx) && (ctx)->dev_fd > 2 && isatty((ctx)->dev_fd))

static void *_serial_read_task(void *arg);

serial_context_t *serial_open(const char *dev) {
    if(!dev || *dev == '\0') return NULL;
    serial_context_t *ctx = malloc(sizeof(serial_context_t));
    if(ctx == NULL) {
        //Error
    }
    ctx->rx.buffer = ring_new(RX_BUFFER_SIZE);
    if(!ctx->rx.buffer) {
        free(ctx);
        //Error
        error(NULL, "Unable to use the memory allocator");
    }
    if((ctx->dev_fd = open(dev, O_RDWR | O_NOCTTY)) < 0) {
        perror(__func__);
        free(ctx->rx.buffer);
        free(ctx);
        return NULL;
    }
    if(!isatty(ctx->dev_fd)) {
        close(ctx->dev_fd);
        free(ctx->rx.buffer);
        free(ctx);
        error(NULL, "Device is not a serial TTY");
    }
    struct termios dev_io;
    tcgetattr(ctx->dev_fd, &dev_io);
    dev_io.c_cflag &= ~(CRTSCTS | PARENB | CSTOPB | CSIZE);
    dev_io.c_iflag &= ~(IXON | IXOFF | IXANY);
    dev_io.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    dev_io.c_oflag = 0;
    dev_io.c_cflag |= CS8 | CREAD | CLOCAL;
    dev_io.c_cc[VMIN] = 1;
    dev_io.c_cc[VTIME] = 30;
    cfsetospeed(&dev_io, BAUD_RATE);
    cfsetispeed(&dev_io, BAUD_RATE);
    tcsetattr(ctx->dev_fd, TCSANOW, &dev_io);
    tcflush(ctx->dev_fd, TCIFLUSH);
    ctx->rx.active = 1;
    if(pthread_create(&ctx->rx.thread, NULL, _serial_read_task, ctx) != 0) {
        close(ctx->dev_fd);
        free(ctx->rx.buffer);
        free(ctx);
        error(NULL; "Could not start RX thread");
    }
    return ctx;
}

int serial_close(serial_context_t *ctx) {
    if(!context_isValid(ctx)) return 1;
    if(ctx->rx.active) {
        pthread_cancel(ctx->rx.thread);
        if(pthread_join(ctx->rx.thread, NULL) != 0) {
            //Error
        }
    }
    if(close(ctx->dev_fd) != 0) {
        //Error
    }
    ring_delete(ctx->rx.buffer);
    free(ctx);
    return 0;
}

size_t serial_read(serial_context_t *ctx, void *dest, size_t size) {
    if(!context_isValid(ctx) || !dest || !serial_read_active(ctx)) {
        return 0;
    }
    size_t n;
    for(n = 0; n < size; ++n) {
        if(ring_pop(ctx->rx.buffer, dest + n) != 0) break;
    }
    return n;
}

unsigned char serial_read_active(serial_context_t *ctx) {
    return context_isValid(ctx) ? ctx->rx.active : 0;
}

size_t serial_read_available(serial_context_t *ctx) {
    return (context_isValid(ctx)) ? ring_used(ctx->rx.buffer) : 0;
}

void serial_read_flush(serial_context_t *ctx) {
    if(!context_isValid(ctx)) return;
    ring_flush(ctx->rx.buffer);
    static const struct timespec flush_sleep_val = { 0, ONE_MSEC };
    nanosleep(&flush_sleep_val, NULL);
    tcflush(ctx->dev_fd, TCIFLUSH);
}

ssize_t serial_send(serial_context_t *ctx, const void *buf, size_t size) {
    ssize_t written = write(ctx->dev_fd, buf, size);
    static const struct timespec write_sleep_val = { 0, ONE_MSEC };
    tcdrain(ctx->dev_fd);
    nanosleep(&write_sleep_val, NULL);
    return written;
}

#define RX_INTER_BUFFER_SIZE 32
static void *_serial_read_task(void *arg) {
    serial_context_t *ctx = arg;
    unsigned char rx_inter_buffer[RX_INTER_BUFFER_SIZE];
    while(1) {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        ssize_t received = read(ctx->dev_fd, rx_inter_buffer, RX_INTER_BUFFER_SIZE);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if(received < 0) perror(__func__);
        else for(size_t i = 0; i < received; ++i) {
            unsigned char c = rx_inter_buffer[i];
            ring_push(ctx->rx.buffer, c);
        }
    }
    pthread_exit(NULL);
}