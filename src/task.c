#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <glb-lib/list.h>

#include "task.h"

struct sg_entry {
    char *data;
    size_t size;
};

static int
add_data_move(struct kmqTask *self, char *data, size_t size)
{
    struct sg_entry *entry;

    if (self->sg_item >= TASK_SG_LIST_LEN) return -1;
    entry = &self->sg_list[self->sg_item];

    entry->data = data;
    entry->size = size;

    ++self->sg_item;

    return 0;
}

static int
add_data_copy(struct kmqTask *self, const char *data, size_t size)
{
    struct sg_entry *entry;
    char *data_copy;

    if (self->sg_item >= TASK_SG_LIST_LEN) return -1;
    entry = &self->sg_list[self->sg_item];

    fprintf(stderr, "debug3: appending '%.*s'\n", (int) size, data);

    data_copy = malloc(size);
    if (!data_copy) return -1;

    memcpy(data_copy, data, size);

    entry->data = data_copy;
    entry->size = size;

    ++self->sg_item;

    return 0;
}

static int
add_buffer(struct kmqTask *self, const char *buf, size_t len)
{
    return -1;
}

static int
add_chunk(struct kmqTask *self, struct chunk *chunk)
{
    return -1;
}

static int
delete(struct kmqTask *self)
{
    if (self->input) self->input->del(self->input);
    if (self->output) self->output->del(self->output);
    if (self->sg_list) free(self->sg_list);

    return 0;
}

int
kmqTask_new(struct kmqTask **task)
{
    struct kmqTask *self;
    int error_code = -1;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->sg_list = calloc(TASK_SG_LIST_LEN, sizeof(self->sg_list));
    if (!self->sg_list) goto error;

    error_code = kmqChunkPipeline_new(&self->input);
    if (error_code != 0) goto error;
    error_code = kmqChunkPipeline_new(&self->output);
    if (error_code != 0) goto error;

    self->add_data_move = add_data_move;
    self->add_data_copy = add_data_copy;
    self->add_buffer = add_buffer;
    self->add_chunk = add_chunk;
    self->del = delete;

    *task = self;
    return 0;
error:
    delete(self);
    return error_code;
}

