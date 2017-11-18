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

struct kmqEndPoint
{
    enum kmqEndPointType type;
    enum kmqEndPointRole role;
    enum kmqEndPointReliability reliability;

    struct list_head remotes;

    int (*set_opt)(struct kmqEndPoint *self, enum kmqEndPointOpt opt, void *value);

    int (*add_remote)(struct kmqEndPoint *self, struct kmqEndPoint *remote);
    int (*send)(struct kmqEndPoint *self, const char *buf, size_t buf_len);

    int (*del)(struct kmqEndPoint *self);
};

int kmqEndPoint_new(struct kmqEndPoint **endpoint,
                    enum kmqEndPointType type,
                    enum kmqEndPointRole role,
                    enum kmqEndPointReliability reliability);
