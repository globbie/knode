#include "timer.h"

static int
init(struct kmqTimer *self)
{
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
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->init = init;
    self->del = delete;

    *timer = self;
    return 0;
error:
    delete(self);
    return -1;
}
