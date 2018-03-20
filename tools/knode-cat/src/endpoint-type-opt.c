#include "endpoint-role-opt.h"

#include <endpoint.h>

#include <string.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    for (size_t i = 0; i < KMQ_ENDPOINT_TYPES_COUNT; ++i) {
        if (strncmp(kmq_endpoint_type_str[i], input, input_len) == 0 && strlen(kmq_endpoint_type_str[i]) == input_len) {
            *((enum kmqEndPointType *) self->data) = (enum kmqEndPointType) i;
            return 0;
        }
    }

    options_status = "unknown role";
    return -1;
}

static int
free__(struct glbOption *self)
{
    (void) self;
    return 0;
}

static int
print__(struct glbOption *self)
{
    printf("\t--%s=%d // fixme: print string representation\n", self->name, *(int *) self->data);
    return 0;
}

struct glbOptType kndEndPointTypeType = {
    .name = "TYPE",
    .has_arg = true,
    .init = NULL,
    .parse = parse__,
    .free = free__,
    .print = print__
};

