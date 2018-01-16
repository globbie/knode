#include "address-opt.h"
#include "utils/addrinfo.h"

#include <stdio.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    int error_code;
    struct addrinfo *addrinfo;

    self->data = NULL;

    error_code = addrinfo_new(&addrinfo, input, input_len);
    if (error_code != 0) return -1;

    self->data = addrinfo;

    return 0;
}

static int
free__(struct glbOption *self)
{
    if (self->data) free(self->data);
    return 0;
}

struct glbOptType kndAddressOptType = {
    .name = "KNODE-ADDRESS",
    .has_arg = true,
    .init = NULL,
    .parse = parse__,
    .free = free__
};


