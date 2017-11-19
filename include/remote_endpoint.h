#pragma once

#include <stdlib.h>

struct kmqRemoteEndPoint
{
    int (*set_address)(struct kmqRemoteEndPoint *self,
                       const char *address, size_t address_len);
    int (*del)(struct kmqRemoteEndPoint *self);
};

int kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote);

