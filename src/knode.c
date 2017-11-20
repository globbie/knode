#include <signal.h>
#include <stdlib.h>

#include "knode.h"
#include "remote_endpoint.h"

static int
init(struct kmqKnode *self)
{
    struct kmqEndPoint *iter;

    list_foreach_entry(iter, struct kmqEndPoint, &self->endpoints, knode_entry) {

        switch (iter->options.role) {
        case KMQ_TARGET:
            iter->listener =\
                evconnlistener_new_bind(self->evbase, iter->accept_cb, iter,
                                        (LEV_OPT_CLOSE_ON_FREE |
                                         LEV_OPT_CLOSE_ON_EXEC |
                                         LEV_OPT_REUSEABLE
                                        ), -1,
                                        iter->options.address->ai_addr,
                                        iter->options.address->ai_addrlen);
            if (!iter->listener) return -1;

            continue;
        case KMQ_INITIATOR:
            // todo: connect
            continue;
        default:
            return -1;
        }
    }

    return 0;
}

static void
signal_cb(int sig, short what __attribute__((unused)), void *arg)
{
    struct kmqKnode *knode = arg; (void) knode;

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
    list_add_tail(&self->endpoints, &endpoint->knode_entry);

    return 0;
}

static int
add_timer(struct kmqKnode *self, struct kmqTimer *timer)
{
    (void) self; (void) timer;
    return 0;
}

static int
dispatch(struct kmqKnode *self)
{
    int error_code;

    error_code = init(self);
    if (error_code != 0) return error_code;

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

    list_head_init(&self->endpoints);

    self->evbase = event_base_new();
    if (!self->evbase) goto error;

    self->signal.sigint = event_new(self->evbase, SIGINT, EV_SIGNAL, signal_cb, self);
    if (!self->signal.sigint) goto error;

    error_code = event_add(self->signal.sigint, NULL);
    if (error_code != 0) goto error;

    self->dispatch = dispatch;
    self->del = delete;
    self->add_endpoint = add_endpoint;
    self->add_timer = add_timer;

    *knode = self;
    return 0;

error:
    delete(self);
    return -1;
}
