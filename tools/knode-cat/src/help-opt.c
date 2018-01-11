#include "options.h"

struct glbOptType kndHelpOptType = {
    .parse = kndHelpOptType_parse,
    .free = kndHelpOptType_free
};

int
kndHelpOptType_parse(struct glbOption *self, const char *input, size_t input_len)
{
    return -1;
}

int
kndHelpOptType_free(struct glbOption *self)
{
    return -1;
}

