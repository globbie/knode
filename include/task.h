#pragma once

#include <glb-lib/list.h>

struct task_chunk
{
    struct list_head task_entry;

    const char *end;
    size_t payload_size;

    char payload[];
};

struct kmqTask
{
    uint32_t task_id;

    size_t chunk_size;
    size_t chunks_count_max;

    struct list_head chunks;
    size_t chunks_count;

    // public interface

    int (*add_buffer)(struct kmqTask *self, const char *buf, size_t size, bool terminate);
    int (*del)(struct kmqTask *self);

    // int (*add_chunk); // for copying fewer times
};

int kmqTask_new(struct kmqTask **task, ...);
