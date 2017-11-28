#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "task.h"

#define DEF_CHUNK_SIZE (1 << 12)

static int
add_buffer(struct kmqTask *self, const char *buf, size_t size, bool terminate)
{
    struct task_chunk *last_chunk;
    size_t data_left = size;
    size_t data_copied = 0;

    last_chunk = list_last_entry(&self->chunks, struct task_chunk, task_entry);

    while (data_left) {
        size_t chunk_free_size;
        size_t size_to_copy;

        if (!last_chunk || last_chunk->payload_size == self->chunk_size) {
            last_chunk = calloc(1, sizeof(*last_chunk) + self->chunk_size);
            if (!last_chunk) return -1;
            list_add_tail(&self->chunks, &last_chunk->task_entry);
        }

        chunk_free_size = self->chunk_size - last_chunk->payload_size;

        // todo: replace it with min/max generic C11 macros
        size_to_copy = chunk_free_size ? chunk_free_size < data_left : data_left;

        memcpy(last_chunk->payload + last_chunk->payload_size, buf + data_copied, size_to_copy);

        last_chunk->payload_size += size_to_copy;
        data_left -= size_to_copy;
        data_copied += size_to_copy;
    }

    // todo: send filled chunks

    return 0;
}

static int
delete(struct kmqTask *self)
{
    return 0;
}

int
kmqTask_new(struct kmqTask **task, ...)
{
    struct kmqTask *self;
    va_list args;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    // todo: allocate first chunk

    va_start(args, task);
    self->chunks_count_max = va_arg(args, size_t);
    va_end(args);

    self->add_buffer = add_buffer;
    self->del = delete;

    *task = self;
    return 0;
}
