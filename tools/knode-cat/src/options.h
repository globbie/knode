#pragma once

struct glbOption;

struct glbOptType
{
    int (*parse)(struct glbOption *option, const char *input, size_t input_len);
    int (*free)(struct glbOption *option);
};

struct glbOption
{
    const char *name;
    size_t name_len;

    char short_name;

    const char *description;
    size_t description_len;

    void *data;

    int (*parse)(struct glbOption *self, const char *input, size_t input_len);
};

