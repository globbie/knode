#include <signal.h>
#include <stdlib.h>

#include "knode.h"

static void
signal_cb(int sig, short what __attribute__((unused)), void *arg)
{
    struct kmqKnode *knode = arg;

    switch (sig) {
    case SIGINT:
        printf("caught SIGINT\n");
        break;
    default:
        break;
    }
}

static int
add_endpoint(struct kmqKnode *self, struct kmqEndPoint *endpoint)
{
    return 0;
}

static int
dispatch(struct kmqKnode *self)
{

    printf("kmqKnode started\n");
    event_base_dispatch(self->evbase);
    printf("kmqKnode stopped\n");

    return 0;
}

static int
delete(struct kmqKnode *self)
{
    if (self->evbase) event_base_free(self->evbase);
    if (self->signal.sigint) event_free(self->signal.sigint);

    free(self);

    return 0;
}

int kmqKnode_new(struct kmqKnode **knode)
{
    struct kmqKnode *self;
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->evbase = event_base_new();
    if (!self->evbase) goto error;

    self->signal.sigint = event_new(self->evbase, SIGINT, EV_SIGNAL, signal_cb, self);
    if (!self->signal.sigint) goto error;

    error_code = event_add(self->signal.sigint, NULL);
    if (error_code != 0) goto error;

    self->dispatch = dispatch;
    self->del = delete;
    self->add_endpoint = add_endpoint;

    *knode = self;
    return 0;

error:
    delete(self);
    return -1;
}
