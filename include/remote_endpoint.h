#pragma once

#include <stdlib.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <glb-lib/list.h>

#include "utils/addrinfo.h"

struct kmqRemoteEndPoint
{
    struct list_head endpoint_entry;

    struct bufferevent *evbuf;

    struct {
        struct addrinfo *address;
    } options;

    int (*send)(struct kmqRemoteEndPoint *self, const char *buf, size_t buf_len);
    int (*init)(struct kmqRemoteEndPoint *self, struct event_base *evbase);
    int (*connect)(struct kmqRemoteEndPoint *self, struct event_base *evbase);
    int (*accept)(struct kmqRemoteEndPoint *self, struct event_base *evbase,
                  evutil_socket_t fd);
    int (*set_address)(struct kmqRemoteEndPoint *self,
                       const char *address, size_t address_len);
    int (*del)(struct kmqRemoteEndPoint *self);
};

int kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote);

