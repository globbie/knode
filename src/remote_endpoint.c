#include "remote_endpoint.h"

static int
set_address(struct kmqRemoteEndPoint *self, const char *address, size_t address_len)
{
    (void) self; (void) address; (void) address_len;
    return 0;
}

static int
delete(struct kmqRemoteEndPoint *self)
{
    free(self);
    return 0;
}

int kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote)
{
    struct kmqRemoteEndPoint *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->set_address = set_address;
    self->del = delete;

    *remote = self;
    return 0;
//error:
//    delete(self);
//    return -1;
}
