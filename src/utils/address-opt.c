#include "address-opt.h"

#include <addrinfo.h>

#include <stdio.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    int error_code;
    struct addrinfo *addrinfo;

    error_code = addrinfo_new(&addrinfo, input, input_len);
    if (error_code != 0) {
        return -1;
    }

    *(struct addrinfo **)self->data = addrinfo;
    return 0;
}

static int
print__(struct glbOption *self)
{
    char hbuf[NI_MAXHOST];
    char sbuf[NI_MAXSERV];

    struct addrinfo *addrinfo = *(struct addrinfo **) self->data;


    if (getnameinfo(addrinfo->ai_addr, addrinfo->ai_addrlen, hbuf,
                    sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
        printf("\t--%s=%s:%s\n", self->name, hbuf, sbuf);
    } else {
        printf("\t--%s=%s\n", self->name, "(unknown)");
    }

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
    .free = free__,
    .print = print__
};


