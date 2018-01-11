#include <stdlib.h>
#include <kmq.h>

#include "options.h"
#include "address-opt.h"

struct options
{
    enum kmqEndPointType endpoint_type;
    enum kmqEndPointRole endpoint_role;
    struct kndAddress *address;
} config = {
    .endpoint_type = KMQ_PUSH,
    .endpoint_role = KMQ_INITIATOR,
    .address = NULL
};

struct glbOption options[] = {

    {
        .name = "type",
        .short_name = 't',
        .description = "Type of endpoint will be used here. Possible values: 'push', 'pull', 'pub', 'sub'",
        .required = false,
        .data = &config.endpoint_type,
        .type = NULL
    },

    {
        .name = "role",
        .short_name = 'r',
        .description = "The role defines will this command initiate connection or wait it. "
                       "Possible values: 'target', 'initiator'",
        .required = false,
        .data = &config.endpoint_role,
        .type = NULL
    },

    {
        .name = "address",
        .short_name = 'a',
        .description = "Address to bind to or to connect to (see role and type)",
        .required = true,
        .data = &config.address,
        .type = &kndAddressOptType
    },

    GLB_OPTS_HELP,
    GLB_OPTS_TERMINATOR
};

int main(int argc, const char **argv)
{
    int error_code;

    error_code = glb_parse_options(options, argc, argv);
    if (error_code != 0) {
        fprintf(stderr, "glb_parse_option() failed, error: '%s'\n", glb_get_options_status());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
