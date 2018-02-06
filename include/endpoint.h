#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <event2/event.h>
#include <event2/listener.h>

#include <glb-lib/list.h>

#include "remote_endpoint.h"
#include "timer.h"
#include "task.h"

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

    struct list_head reconnect_remotes;

    struct event_base *evbase;
    struct evconnlistener *listener;

    struct kmqTimer *heartbeat;

    struct {
        enum kmqEndPointType type;
        enum kmqEndPointRole role;
        enum kmqEndPointReliability reliability;

        const struct addrinfo *address;

        int (*callback)(struct kmqEndPoint *self, struct kmqTask *task);
    } options;

    // public interface

    int (*init)(struct kmqEndPoint *self, struct event_base *evbase);
    int (*del)(struct kmqEndPoint *self);

    int (*set_address)(struct kmqEndPoint *self, const struct addrinfo *addrinfo);
    int (*add_remote)(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote);

    int (*schedule_task)(struct kmqEndPoint *self, struct kmqTask *task);

    // private interface
    void (*accept_cb)(struct evconnlistener *listener, evutil_socket_t fd,
                      struct sockaddr *addr, int len, void *arg);
};

int kmqEndPoint_new(struct kmqEndPoint **endpoint);

