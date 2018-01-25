#include "options.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char *options_status = "(none)";
const char *program_name = NULL;

enum parser_state {
    ps_init,
    ps_option_start,

    ps_short_option_key,
    ps_short_option_start,
    ps_short_option,

    ps_long_option_key_start,
    ps_long_option_key,

    ps_long_option_value_start,
    ps_long_option_value
};

static int
find_short_option(struct glbOption *options, char short_option, struct glbOption **option,
                  enum parser_state *state __attribute__((unused)))
{
    for (struct glbOption *i = options; i->short_name != '\0'; ++i) {
        if (i->short_name != short_option) continue;

        *option = i;
        if (!i->type->has_arg) return i->type->init(i, options);
        return 0;
    }

    options_status = "given short option is unknown";
    return -1;
}

static int
find_long_option(struct glbOption *options, const char *long_option, size_t long_option_len, struct glbOption **option,
                 enum parser_state *state __attribute__((unused)))
{
    for (struct glbOption *i = options; i->name; ++i) {
        if (strncmp(i->name, long_option, long_option_len) != 0 || i->name_len != long_option_len) continue;

        *option = i;
        if (!i->type->has_arg) return i->type->init(i, options);
        return 0;
    }

    options_status = "given long option is unknown";
    return -1;
}

const char *
glb_get_options_status(void)
{
    return options_status;
}

int
glb_options_print(struct glbOption *options)
{
    for (struct glbOption *i = options; i->name; ++i) {
        if (i->type->print) i->type->print(i);
    }

    return 0;
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

    struct glbOption *option = NULL;

    int error_code;


    program_name = argv[0];

    for (size_t i = 1; i < (size_t) argc; ++i) {
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
                    state = ps_short_option_key;

                    short_option = *cursor;
                    goto next_char;
                }

                options_status = "'-' must be followed by another '-' or alpha character";
                return -1;

            case ps_short_option_key:
                if (*cursor != '\0') {
                    options_status = "short option must be exactly one character";
                    return -1;
                }

                error_code = find_short_option(options, short_option, &option, &state);
                if (error_code != 0) return -1;

                state = ps_short_option_start;

                if (option->type->has_arg && i == (size_t) argc - 1) {
                    options_status = "options argument expected";
                    return -1;
                }

                goto next_token;

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

                error_code = option->type->parse(option, value, value_len);
                if (error_code != 0) return -1;

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

                if (*cursor == '\0' || *cursor == '=') {
                    error_code = find_long_option(options, long_option, long_option_len, &option, &state);
                    if (error_code == -1) return -1;
                }

                if (*cursor == '\0') {
                    if (option->type->has_arg) {
                        options_status = "option must have an argument";
                        return -1;
                    }
                    goto next_token;
                }

                if (*cursor == '=') {
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

                {
                    int error_code;
                    error_code = option->type->parse(option, value, value_len);
                    if (error_code != 0) return -1;
                }

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

