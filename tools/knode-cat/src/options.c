#include <stdio.h>
#include <ctype.h>

#include "options.h"

static const char *options_status = "(none)";

enum parser_state {
    ps_init,
    ps_option_start,

    ps_short_option_start,
    ps_short_option,


    ps_long_option_key_start,
    ps_long_option_key,
    ps_long_option_value_start,
    ps_long_option_value
};

static int
show_help_message(struct glbOption *options, int argc, const char **argv)
{
    return -1;
}

const char *
glb_get_options_status(void)
{
    return options_status;
}

int
glb_parse_options(struct glbOption *options, int argc, const char **argv)
{
    enum parser_state state = ps_init;

    char short_option = '\0';

    const char *long_option = NULL;
    size_t long_option_len;

    const char *value = NULL;
    size_t value_len = 0;

    for (size_t i = 1; i < argc; ++i) {
        const char *token = argv[i];
        const char *cursor = token;

        while (true) {

            switch (state) {

            case ps_init:
                if (*cursor == '-') { state = ps_option_start; goto next_char; }

                options_status = "unexpected option start";
                return -1;

            case ps_option_start:
                if (*cursor == '-') { state = ps_long_option_key_start; goto next_char; }

                if (isalpha(*cursor)) {
                    state = ps_short_option_start;

                    if (i == argc - 1) {
                        options_status = "options argument expected";
                        return -1;
                    }

                    short_option = *cursor;

                    printf("got short option: '%c'\n", short_option);
                    goto next_token;
                }

                options_status = "'-' must be followed by another '-' or alpha character";
                return -1;

            case ps_short_option_start:
                state = ps_short_option;

                value = cursor;
                value_len = 1;

                goto next_char;

            case ps_short_option:
                if (*cursor != '\0') {
                    ++value_len;
                    goto next_char;
                }

                printf("\tgot short option value: '%.*s'\n", (int) value_len, value);
                state = ps_init;
                goto next_token;

            case ps_long_option_key_start:

                if (isalpha(*cursor)) {
                    state = ps_long_option_key;

                    long_option = cursor;
                    long_option_len = 1;

                    goto next_char;
                }

                options_status = "'--' must be followed by alpha character";
                return -1;

            case ps_long_option_key:
                if (isalnum(*cursor)) {
                    ++long_option_len;
                    goto next_char;
                }

                if (*cursor == '=') {
                    printf("got long option: '%.*s'\n", (int) long_option_len, long_option);
                    state = ps_long_option_value_start;
                    goto next_char;
                }

                options_status = "long option must be followed by '=' character";
                return -1;

            case ps_long_option_value_start:
                if (*cursor != '\0') {
                    value = cursor;
                    value_len = 1;
                    state = ps_long_option_value;
                    goto next_char;
                }

                options_status = "long option value cannot be empty";
                return -1;

            case ps_long_option_value:
                if (*cursor != '\0') {
                    ++value_len;
                    goto next_char;
                }

                printf("\tgot long option value: '%.*s'\n", (int) value_len, value);
                state = ps_init;
                goto next_token;

            default:
                options_status = "internal error";
                return -1;
            } // switch

next_char:
            ++cursor;
        } // while
next_token:
        ;
    } // for

    return 0;
}
