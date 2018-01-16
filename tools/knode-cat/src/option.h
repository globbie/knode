#pragma once

#include <stdbool.h>
#include <stdlib.h>

struct glbOptType;

struct glbOption
{
    const char *name;
    size_t      name_len;

    char        short_name;
    const char *description;

    bool        required;
    void       *data;

    struct glbOptType*   type;
};
