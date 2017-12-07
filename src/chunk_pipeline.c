#include <stdlib.h>
#include <string.h>

#include "chunk_pipeline.h"
#include "utils/math.h"

#define DEF_CHUNK_SIZE (1 << 12)

static int
add_chunk(struct kmqChunkPipeline *self, struct chunk *chunk)
{
    list_add_tail(&self->chunks, &chunk->chunks_entry);
    ++self->chunks_count;

    return 0;
}

static int
add_buffer(struct kmqChunkPipeline *self, const char *buf, size_t size)
{
    struct chunk *last_chunk;

    size_t data_left = size;
    size_t data_copied = 0;

    last_chunk = list_last_entry(&self->chunks, struct chunk, chunks_entry);

    while (data_left) {
        size_t chunk_free_size;
        size_t copy_size;

        if (!last_chunk || last_chunk->payload_size == self->chunk_size) {
            last_chunk = calloc(1, sizeof(*last_chunk) + self->chunk_size);
            if (!last_chunk) return -1;

            list_add_tail(&self->chunks, &last_chunk->chunks_entry);
            ++self->chunks_count;
        }

        chunk_free_size = self->chunk_size - last_chunk->payload_size;

        copy_size = MIN(chunk_free_size, data_left);

        memcpy(last_chunk->payload + last_chunk->payload_size, buf + data_copied, copy_size);

        last_chunk->payload_size += copy_size;
        data_left -= copy_size;
        data_copied += copy_size;
    }

    // todo: call send callbacks or handle callbacks

    return 0;
}

static int
delete(struct kmqChunkPipeline *self)
{
    free(self);
    return 0;
}

int
kmqChunkPipeline_new(struct kmqChunkPipeline **pipeline)
{
    struct kmqChunkPipeline *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->chunks_count_max = 10; // todo: fix hardcoded magic number

    self->add_buffer = add_buffer;
    self->add_chunk = add_chunk;
    self->del = delete;
    self->chunk_size = DEF_CHUNK_SIZE;

    *pipeline = self;

    return 0;;
}

