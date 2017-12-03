#pragma once

#include <glb-lib/list.h>

struct chunk
{
    struct list_head chunks_entry;
    size_t payload_size;

    char payload[];
};

struct kmqChunkPipeline
{
    struct list_head chunks;
    size_t chunks_count;

    size_t chunk_size;
    size_t chunks_count_max;

    // public interface

    int (*add_buffer)(struct kmqChunkPipeline *self, const char *buf, size_t size);
    int (*add_chunk)(struct kmqChunkPipeline *self, struct chunk *chunk);
    int (*terminate)(struct kmqChunkPipeline *self);

    int (*del)(struct kmqChunkPipeline *self);
};

int kmqChunkPipeline_new(struct kmqChunkPipeline **pipeline);

