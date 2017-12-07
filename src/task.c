#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "task.h"

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

    return 0;
}

int
kmqTask_new(struct kmqTask **task)
{
    struct kmqTask *self;
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    error_code = kmqChunkPipeline_new(&self->input);
    if (error_code != 0) goto error;
    error_code = kmqChunkPipeline_new(&self->output);
    if (error_code != 0) goto error;

    self->add_buffer = add_buffer;
    self->add_chunk = add_chunk;
    self->del = delete;

    *task = self;
    return 0;
error:
    delete(self);
    return error_code;
}

