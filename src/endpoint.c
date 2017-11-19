#include "endpoint.h"

static int
set_address(struct kmqEndPoint *self, const char *address, size_t address_len)
{
    return 0;
}

static int
add_remote(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote)
{
    return 0;
}

static int
init(struct kmqEndPoint *self)
{
    return 0;
}

static int
delete(struct kmqEndPoint *self)
{
    free(self);
    return 0;
}

int kmqEndPoint_new(struct kmqEndPoint **endpoint)
{
    struct kmqEndPoint *self;
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->set_address = set_address;
    self->add_remote = add_remote;
    self->init = init;
    self->del = delete;

    *endpoint = self;
    return 0;
error:
    delete(self);
    return -1;
}
