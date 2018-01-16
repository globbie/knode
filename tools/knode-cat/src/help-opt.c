#include "options.h"

#include <stdio.h>

static int
init__(struct glbOption *self __attribute__((unused)), struct glbOption *options)
{
    printf("Usage: %s [options...]\n\n", program_name);
    printf("Here is the full list of possible options:\n\n");

    for (struct glbOption *i = options; i->name; ++i) {

        if (i->short_name != '\0') printf("\t-%c, ", i->short_name);
        else printf("\t");

        printf("--%s", i->name);

        if (i->type->has_arg) printf("=<%s>", i->type->name);
        else printf("\t\t");

        printf("\t%s", i->description);

        printf("\n");
    }

    return 0;
}

struct glbOptType kndHelpOptType = {
    .name = "HELP-MESSAGE",
    .has_arg = false,
    .init = init__,
    .parse = NULL,
    .free = NULL
};


