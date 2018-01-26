#pragma once

#include "chunk_pipeline.h"

#define TASK_SG_LIST_LEN 100

struct sg_entry;

struct kmqTask
{
    uint32_t task_id;

    struct sg_entry *sg_list;
    size_t sg_items_count;

    size_t size;

    // public interface

    int (*del)(struct kmqTask *self);

    int (*add_data_move)(struct kmqTask *self, char *data, size_t size);
    int (*add_data_copy)(struct kmqTask *self, const char *data, size_t size);

    int (*get_data)(struct kmqTask *self, size_t i, const char **data, size_t *size);
};

int kmqTask_new(struct kmqTask **task);

