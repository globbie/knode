#pragma once

struct kmqRemoteEndPoint
{
    int (*del)(struct kmqRemoteEndPoint *self);
};

int kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote)
