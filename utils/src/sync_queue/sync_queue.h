#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct
{
    t_queue *queue;
    pthread_mutex_t mutex;
} t_sync_queue;

t_sync_queue *sync_queue_create();

void queue_sync_push(t_sync_queue *self, void *element);

void *queue_sync_pop(t_sync_queue *self);

void sync_queue_destroy(t_sync_queue* self);

#endif