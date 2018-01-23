#include "knode-cat.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <event.h>

static void event_handler(int fd, short event, void *arg)
{
    struct kmqKnodeCat *self = arg;

    if (event & EV_READ) {
        char buffer[50];
        ssize_t read_len;

        read_len = read(fd, buffer, sizeof(buffer));
        if (read_len < 0) {
            fprintf(stderr, "read error\n");
            //self->stop(self);
        } else if (read_len == 0) {
            printf("end of file\n");
            event_del(self->stdio_event);
            return;
        }

        buffer[read_len] = '\0';
        printf("<<<%s>>>\n", buffer);
        // todo: form task and send it
    }
}

static int
start__(struct kmqKnodeCat *self)
{
    int error_code = -1;

    error_code = self->knode->dispatch(self->knode);

    return error_code;
}

static int
del__(struct kmqKnodeCat *self)
{
    if (self->remote) self->remote->del(self->remote);
    if (self->endpoint) self->endpoint->del(self->endpoint);
    if (self->knode) self->knode->del(self->knode);

    free(self);

    return 0;
}

int
kmqKnodeCat_new(struct kmqKnodeCat **service, const struct kmqKnodeCatConfig *config)
{
    struct kmqKnodeCat *self = calloc(1, sizeof(*self));
    int error_code;
    if (!self) return -1;

    self->config = config;

    error_code = kmqKnode_new(&self->knode);
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&self->endpoint);
    if (error_code != 0) goto error;

    self->endpoint->options.type = self->config->endpoint_type;

    if (self->config->endpoint_role == KMQ_INITIATOR) {
        self->endpoint->options.role = KMQ_INITIATOR;

        error_code = kmqRemoteEndPoint_new(&self->remote);
        if (error_code != 0) goto error;

        error_code = self->remote->set_address(self->remote, self->config->address);
        if (error_code != 0) goto error;

        error_code = self->endpoint->add_remote(self->endpoint, self->remote);
        if (error_code != 0) goto error;

    } else {
        self->endpoint->options.role = KMQ_TARGET;

        error_code = self->endpoint->set_address(self->endpoint, self->config->address);
        if (error_code != 0) goto error;
    }

    error_code = self->knode->add_endpoint(self->knode, self->endpoint);
    if (error_code != 0) goto error;

    if (self->endpoint->options.type == KMQ_PUSH) {
        self->stdio_event = event_new(self->knode->evbase, 0, EV_READ | EV_PERSIST, event_handler, self);
        event_add(self->stdio_event, NULL);
    }

    self->del = del__;
    self->start = start__;

    *service = self;
    return 0;
error:
    del__(self);
    return error_code;
}
