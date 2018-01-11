#pragma once

#include "option.h"
#include "address-opt.h"
#include "help-opt.h"

#define GLB_OPTS_HELP { "help", 'h', "Show help message", false, NULL, &kndHelpOptType }
#define GLB_OPTS_TERMINATOR { NULL, 0, NULL, false, NULL, NULL }

#define GLB_OPTS_COUNT_MAX 512

struct glbOptType
{
    int (*parse)(struct glbOption *option, const char *input, size_t input_len);
    int (*free)(struct glbOption *option);
};

int glb_parse_options(struct glbOption *options, int argc, const char **argv);
const char *glb_get_options_status(void);


