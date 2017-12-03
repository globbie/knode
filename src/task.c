#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "task.h"


static int
delete(struct kmqTask *self)
{
    return 0;
}

int
kmqTask_new(struct kmqTask **task)
{
    struct kmqTask *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->del = delete;

    *task = self;
    return 0;
}
