#pragma once

#include <stdbool.h>
#include <stdlib.h>

struct glbOption;

struct glbOptType
{
    int (*parse)(struct glbOption *option, const char *input, size_t input_len);
    int (*free)(struct glbOption *option);
};

struct glbOption
{
    const char *name;
    char        short_name;
    const char *description;
    bool        required;

    void       *data;

    struct glbOptType* type;
};

int glb_parse_options(struct glbOption *options, int argc, const char **argv);

