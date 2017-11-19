#pragma once

struct kmqTimer
{
    struct {
        int tick;
    } options;

    int (*callback)(struct kmqTimer *self, void *cb_arg);
    void *callback_arg;

    int (*init)(struct kmqTimer *self);
    int (*del)(struct kmqTimer *self);
};

int kmqTimer_new(struct kmqTimer **timer);
