#pragma once

#include <stdbool.h>

#include <event2/event.h>

#include "endpoint.h"

struct kmqKnode
{
    struct event_base *evbase;

    struct {
        struct event *sigint;
    } signal;

    int (*dispatch)(struct kmqKnode *self);
    int (*add_endpoint)(struct kmqKnode *self, struct kmqEndPoint *endpoint);
    int (*del)(struct kmqKnode *self);
};

int kmqKnode_new(struct kmqKnode **knode);

