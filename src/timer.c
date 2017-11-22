#include <stdlib.h>

#include "timer.h"

static void
event_cb(int fd __attribute__((unused)), short whar __attribute__((unused)),
         void *arg)
{
    struct kmqTimer *self = arg;
    self->callback(self, self->callback_arg);
}

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

    self->event_cb = event_cb;
    self->init = init;
    self->del = delete;

    *timer = self;
    return 0;
}
