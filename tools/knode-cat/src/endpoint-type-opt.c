#include "endpoint-role-opt.h"
#include "options.h"

#include <endpoint.h>

#include <string.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    if (strncmp("push", input, input_len) == 0 && sizeof("push") - 1 == input_len)
        *((enum kmqEndPointType *) self->data) = KMQ_PUSH;
    else if (strncmp("pull", input, input_len) == 0 && sizeof("pull") - 1 == input_len)
        *((enum kmqEndPointType *) self->data) = KMQ_PULL;
    else if (strncmp("pub", input, input_len) == 0 && sizeof("pub") - 1 == input_len)
        *((enum kmqEndPointType *) self->data) = KMQ_PUB;
    else if (strncmp("sub", input, input_len) == 0 && sizeof("sub") - 1 == input_len)
        *((enum kmqEndPointType *) self->data) = KMQ_SUB;
    else {
        options_status = "unknown role";
        return -1;
    }

    return 0;
}

static int
free__(struct glbOption *self)
{
    (void) self;
    return 0;
}

struct glbOptType kndEndPointTypeType = {
    .name = "TYPE",
    .has_arg = true,
    .init = NULL,
    .parse = parse__,
    .free = free__
};

