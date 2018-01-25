#pragma once

#include "chunk_pipeline.h"

#define TASK_SG_LIST_LEN 100

struct sg_entry;

struct kmqTask
{
    uint32_t task_id;

    struct kmqChunkPipeline *input;
    struct kmqChunkPipeline *output;

    struct sg_entry *sg_list;
    size_t sg_item;

    // public interface

    int (*add_data_move)(struct kmqTask *self, char *data, size_t size);
    int (*add_data_copy)(struct kmqTask *self, const char *data, size_t size);

    int (*add_buffer)(struct kmqTask *self, const char *buf, size_t len);
    int (*add_chunk)(struct kmqTask *self, struct chunk *chunk);

    int (*terminate)(struct kmqTask *self);

    int (*del)(struct kmqTask *self);
};

int kmqTask_new(struct kmqTask **task);

