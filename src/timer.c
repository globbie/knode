#include <stdlib.h>

#include "timer.h"

static int
init(struct kmqTimer *self)
{
    (void) self;
    return 0;
}

static int
delete(struct kmqTimer *self)
{
    free(self);
    return 0;
}

int kmqTimer_new(struct kmqTimer **timer)
{
    struct kmqTimer *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->init = init;
    self->del = delete;

    *timer = self;
    return 0;
}
