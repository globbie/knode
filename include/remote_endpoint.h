#pragma once

#include <stdlib.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <glb-lib/list.h>

#include "utils/addrinfo.h"
#include "task.h"

enum kmqEndPointEvent
{
    KMQ_EPEVENT_CONNECTED,
    KMQ_EPEVENT_ERROR,
    KMQ_EPEVENT_DISCONNECTED
};

struct kmqRemoteEndPoint
{
    struct list_head endpoint_entry;
    struct bufferevent *evbuf;

    struct {
        const struct addrinfo *address;
    } options;

    int (*read_cb)(struct kmqRemoteEndPoint *remote, struct kmqTask *task, void *cb_arg);
    int (*event_cb)(struct kmqRemoteEndPoint *remote, enum kmqEndPointEvent event, void *cb_arg);

    void *cb_arg;

    // public iterface

    int (*del)(struct kmqRemoteEndPoint *self);
    int (*set_address)(struct kmqRemoteEndPoint *self,
                       const struct addrinfo *address);

    // private interface

    int (*init)(struct kmqRemoteEndPoint *self, struct event_base *evbase);
    int (*send)(struct kmqRemoteEndPoint *self, struct kmqTask *task);
    int (*connect)(struct kmqRemoteEndPoint *self, struct event_base *evbase);
    int (*accept)(struct kmqRemoteEndPoint *self, struct event_base *evbase,
                  evutil_socket_t fd);
};

int kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote);

