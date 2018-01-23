#pragma once

#include <kmq.h>

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

    // public methods

    int (*del)(struct kmqKnodeCat *self);
    int (*start)(struct kmqKnodeCat *self);
};

int kmqKnodeCat_new(struct kmqKnodeCat **service, const struct kmqKnodeCatConfig *config);
