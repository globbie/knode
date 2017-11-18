#include "endpoint.h"

static int
delete(struct kmqEndPoint *self)
{
    free(self);
}

int kmqEndPoint_new(struct kmqEndPoint **endpoint,
                    enum kmqEndPointType type,
                    enum kmqChannelType channel_type,
                    const char *address, size_t address_len)
{
    struct kmqEndPoint *self;
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    if (type == KMQ_REQ || type == KMQ_PUSH || type == KMQ_SUB) {

    } else if (type == KMQ_REP || type == KMQ_PULL || KMQ_PUB) {

    } else goto error;

    self->type = type;
    self->del = delete;

    *endpoint = self;
    return 0;
error:
    delete(self);
    return -1;
}
