#include <stdlib.h>
#include <kmq.h>

#include "options.h"
struct options
{
    enum kmqEndPointType endpoint_type;
    enum kmqEndPointRole endpoint_role;
    struct kndAddress *address;
} options = {
    .endpoint_type = KMQ_PUSH,
    .endpoint_role = KMQ_INITIATOR,
    .address = NULL
};

const struct glbOption *option = {
    NULL
};

int main(int argc, const char **arg)
{
    return EXIT_FAILURE;
}
