#pragma once

#include <stdbool.h>

#include <glb-lib/list.h>

#include <event2/event.h>
#include <event2/listener.h>

#include "endpoint.h"
#include "timer.h"

struct kmqKnode
{
    struct event_base *evbase;

    struct {
        struct event *sigint;
    } signal;

    struct list_head endpoints;

    int (*dispatch)(struct kmqKnode *self);

    int (*add_endpoint)(struct kmqKnode *self, struct kmqEndPoint *endpoint);
    int (*add_timer)(struct kmqKnode *self, struct kmqTimer *timer);

    int (*del)(struct kmqKnode *self);
};

int kmqKnode_new(struct kmqKnode **knode);

