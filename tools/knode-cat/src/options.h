#pragma once

#include "option.h"
#include "address-opt.h"
#include "endpoint-role-opt.h"
#include "endpoint-type-opt.h"
#include "help-opt.h"

#define GLB_OPTS_HELP { "help", sizeof("help") - 1, 'h', "Show help message", false, NULL, &kndHelpOptType }
#define GLB_OPTS_TERMINATOR { NULL, 0, 0, NULL, false, NULL, NULL }

#define GLB_OPTS_COUNT_MAX 512

extern const char *options_status;
extern const char *program_name;

struct glbOptType
{
    const char *name;
    bool        has_arg;

    int (*init)(struct glbOption *option, struct glbOption *options);
    int (*parse)(struct glbOption *option, const char *input, size_t input_len);
    int (*free)(struct glbOption *option);
    int (*print)(struct glbOption *option);
};

int glb_parse_options(struct glbOption *options, int argc, const char **argv);
const char *glb_get_options_status(void);

int glb_options_print(struct glbOption *options);


