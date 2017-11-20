#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <glb-lib/list.h>

#include "remote_endpoint.h"

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
    struct {
        enum kmqEndPointType type;
        enum kmqEndPointRole role;
        enum kmqEndPointReliability reliability;
    } options;

    int (*callback)(struct kmqEndPoint *self, const char *buf, size_t buf_len);

    int (*send)(struct kmqEndPoint *self, const char *buf, size_t buf_len);
    int (*set_address)(struct kmqEndPoint *self, const char *address, size_t address_len);
    int (*add_remote)(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote);

    int (*init)(struct kmqEndPoint *self);
    int (*del)(struct kmqEndPoint *self);
};

int kmqEndPoint_new(struct kmqEndPoint **endpoint);

