#include <stdlib.h>

#include <glb-lib/list.h>
#include <glb-lib/options.h>

#include "address-opt.h"
#include "knode-cat.h"
#include "address-opt.h"
#include "endpoint-role-opt.h"
#include "endpoint-type-opt.h"

struct kmqKnodeCatConfig config = {
    .endpoint_type = KMQ_PUSH,
    .endpoint_role = KMQ_INITIATOR,
    .address = NULL
};

struct glbOption options[] = {

    {
        .name = "type",
        .name_len = sizeof("type") - 1,
        .short_name = 't',
        .description = "Type of endpoint will be used here. Possible values: 'push', 'pull', 'pub', 'sub'",
        .required = false,
        .data = &config.endpoint_type,
        .type = &kndEndPointTypeType
    },

    {
        .name = "role",
        .name_len = sizeof("role") - 1,
        .short_name = 'r',
        .description = "The role defines will this command initiate connection or wait it. "
                       "Possible values: 'target', 'initiator'",
        .required = false,
        .data = &config.endpoint_role,
        .type = &kndEndPointRoleType
    },

    {
        .name = "address",
        .name_len = sizeof("address") - 1,
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
    int ret = EXIT_FAILURE;
    struct kmqKnodeCat *service;

    error_code = glb_parse_options(options, argc, argv);
    if (error_code != 0) {
        fprintf(stderr, "glb_parse_option() failed, error: '%s'\n", glb_get_options_status());
        return EXIT_FAILURE;
    }

    //glb_options_print(options);

    error_code = kmqKnodeCat_new(&service, &config);
    if (error_code != 0) goto exit;

    error_code = service->start(service);
    if (error_code != 0) {
        fprintf(stderr, "service finished with failure\n");
        goto exit;
    }

    ret = EXIT_SUCCESS;
exit:
    if (service) service->del(service);
    return ret;
}

