#include "address-opt.h"

struct glbOptType kndAddressOptType = {
    .parse = kndAddressOpt_parse,
    .free = kndAddressOpt_free
};

int
kndAddressOpt_parse(struct glbOption *self, const char *input, size_t input_len)
{
    return -1;
}

int
kndAddressOpt_free(struct glbOption *self)
{
    struct kndAddress *address = self->data;
    //if (address) address->free(address);
    return 0;
}


