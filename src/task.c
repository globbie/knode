#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <glb-lib/list.h>

#include "task.h"

struct sg_entry {
    char   *data;
    size_t  size;
};

static int
add_data_move(struct kmqTask *self, char *data, size_t size)
{
    struct sg_entry *entry;

    if (self->sg_items_count >= TASK_SG_LIST_LEN) return -1;
    entry = &self->sg_list[self->sg_items_count];

    entry->data = data;
    entry->size = size;

    ++self->sg_items_count;
    self->size += size;

    return 0;
}

static int
add_data_copy(struct kmqTask *self, const char *data, size_t size)
{
    struct sg_entry *entry;
    char *data_copy;

    if (self->sg_items_count >= TASK_SG_LIST_LEN) return -1;
    entry = &self->sg_list[self->sg_items_count];

    data_copy = malloc(size);
    if (!data_copy) return -1;

    memcpy(data_copy, data, size);

    entry->data = data_copy;
    entry->size = size;

    ++self->sg_items_count;
    self->size += size;

    return 0;
}

static int
get_data(struct kmqTask *self, size_t i, const char **data, size_t *size)
{
    *data = self->sg_list[i].data;
    *size = self->sg_list[i].size;
    return 0;
}

static int
delete(struct kmqTask *self)
{
    for (size_t i = 0; i < self->sg_items_count; ++i) {
        free(self->sg_list[i].data);
    }

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

    self->move_data = add_data_move;
    self->copy_data = add_data_copy;
    self->get_data = get_data;
    self->del = delete;

    *task = self;
    return 0;
error:
    delete(self);
    return error_code;
}

