#pragma once

#include <event2/event.h>

#include <glb-lib/list.h>

struct kmqTimer
{
    struct list_head knode_entry;
    struct event *event;

    struct {
        struct timeval interval;
    } options;

    int (*callback)(struct kmqTimer *self, void *cb_arg);
    void *callback_arg;

    void (*event_cb)(int fd, short what, void *arg);
    int (*init)(struct kmqTimer *self);
    int (*del)(struct kmqTimer *self);
};

int kmqTimer_new(struct kmqTimer **timer);
