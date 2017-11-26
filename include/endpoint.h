#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <event2/event.h>
#include <event2/listener.h>

#include <glb-lib/list.h>

#include "remote_endpoint.h"
#include "utils/addrinfo.h"

enum kmqEndPointType
{
    KMQ_PUSH, KMQ_PULL,
    KMQ_PUB,  KMQ_SUB
};

enum kmqEndPointRole
{
    KMQ_TARGET, KMQ_INITIATOR
};

enum kmqEndPointReliability
{
    KMQ_ACK_OFF,
    MKQ_ACK_ON
};

struct kmqEndPoint
{
    struct list_head knode_entry;

    struct list_head remotes;

    struct event_base *evbase;
    struct evconnlistener *listener;

    struct {
        enum kmqEndPointType type;
        enum kmqEndPointRole role;
        enum kmqEndPointReliability reliability;

        struct addrinfo *address;

        int (*callback)(struct kmqEndPoint *self, const char *buf, size_t buf_len);
    } options;

    // public interface

    int (*init)(struct kmqEndPoint *self, struct event_base *evbase);
    int (*del)(struct kmqEndPoint *self);

    int (*set_address)(struct kmqEndPoint *self, const char *address, size_t address_len);
    int (*add_remote)(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote);

    int (*send)(struct kmqEndPoint *self, const char *buf, size_t buf_len);

    // private interface
    void (*accept_cb)(struct evconnlistener *listener, evutil_socket_t fd,
                      struct sockaddr *addr, int len, void *arg);
};

int kmqEndPoint_new(struct kmqEndPoint **endpoint);

