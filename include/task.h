#pragma once

#include "chunk_pipeline.h"

struct kmqTask
{
    uint32_t task_id;

    struct kmqChunkPipeline *input;
    struct kmqChunkPipeline *output;

    // public interface

    int (*add_buffer)(struct kmqTask *self, const char *buf, size_t len);
    int (*add_chunk)(struct kmqTask *self, struct chunk *chunk);

    int (*terminate)(struct kmqTask *self);

    int (*del)(struct kmqTask *self);
};

int kmqTask_new(struct kmqTask **task);

