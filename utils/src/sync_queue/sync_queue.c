#include <sync_queue.h>

t_sync_queue *sync_queue_create()
{
    t_sync_queue *s_queue = malloc(sizeof(t_sync_queue));
    s_queue->queue = queue_create();
    pthread_mutex_init(&s_queue->mutex, NULL);
}

void queue_sync_push(t_sync_queue *self, void *element)
{
    pthread_mutex_lock(&self->mutex);
    queue_push(self->queue, element);
    pthread_mutex_unlock(&self->mutex);
}

void *queue_sync_pop(t_sync_queue *self)
{
    pthread_mutex_lock(&self->mutex);
    void *elem = queue_pop(self->queue);
    pthread_mutex_unlock(&self->mutex);
    return elem;
}

void sync_queue_destroy(t_sync_queue* self){
    pthread_mutex_destroy(&self->mutex);
    queue_destroy(self->queue);
    free(self);
}