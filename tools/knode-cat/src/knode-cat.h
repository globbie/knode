#pragma once

#include <kmq.h>

#define STDIN_BUFFER_SIZE 3

struct kmqKnodeCatConfig
{
    enum kmqEndPointType endpoint_type;
    enum kmqEndPointRole endpoint_role;
    const struct addrinfo *address;
};

struct kmqKnodeCat
{
    const struct kmqKnodeCatConfig *config;

    struct kmqKnode *knode;
    struct kmqRemoteEndPoint *remote;
    struct kmqEndPoint *endpoint;

    struct event *stdio_event;


    struct kmqTask *current_task;

    // public methods

    int (*del)(struct kmqKnodeCat *self);
    int (*start)(struct kmqKnodeCat *self);
    int (*stop)(struct kmqKnodeCat *self);
    int (*get_task)(struct kmqKnodeCat *self, struct kmqTask **task);
};

int kmqKnodeCat_new(struct kmqKnodeCat **service, const struct kmqKnodeCatConfig *config);

