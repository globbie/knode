#pragma once

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

enum kmqEndPointOption
{
    KMQ_ENDPOINT_TYPE,
    KMQ_ENDPOINT_ROLE,
    KMQ_ENDPOINT_RELIABILITY
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

    int (*set_option)(struct kmqEndPoint *self, enum kmqEndPointOption opt, int value);
    int (*set_address)(struct kmqEndPoint *self, const char *address, size_t address_len);
    int (*add_remote)(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote);

    int (*del)(struct kmqEndPoint *self);
};

int kmqEndPoint_new(struct kmqEndPoint **endpoint);
