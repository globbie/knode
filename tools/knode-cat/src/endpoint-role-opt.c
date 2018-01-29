#include "endpoint-role-opt.h"

#include <endpoint.h>

#include <string.h>

static int
parse__(struct glbOption *self, const char *input, size_t input_len)
{
    if (strncmp("target", input, input_len) == 0 && sizeof("target") - 1 == input_len) {
        *((enum kmqEndPointRole *) self->data) = KMQ_TARGET;
    } else if (strncmp("initiator", input, input_len) == 0 && sizeof("initiator") - 1 == input_len) {
        *((enum kmqEndPointRole *) self->data) = KMQ_INITIATOR;
    } else {
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

static int
print__(struct glbOption *self)
{
    printf("\t--%s=%d // fixme: print string representation\n", self->name, *(int *) self->data);
    return 0;
}

struct glbOptType kndEndPointRoleType = {
    .name = "ROLE",
    .has_arg = true,
    .init = NULL,
    .parse = parse__,
    .free = free__,
    .print = print__
};

