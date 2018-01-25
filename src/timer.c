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
init(struct kmqTimer *self, struct event_base *evbase)
{
    int error_code;

    fprintf(stderr, "debug2: initializing Timer\n");

    self->event = event_new(evbase, -1, EV_PERSIST, event_cb, self);
    if (!self->event) return -1;

    error_code = evtimer_add(self->event, &self->options.interval);
    if (error_code != 0) goto error;

    return 0;
error:
    event_free(self->event);
    return -1;
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
