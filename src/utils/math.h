#pragma once

static inline size_t min_size_t(size_t x, size_t y)
{
    return x < y ? x : y;
}

static inline size_t max_size_t(size_t x, size_t y)
{
    return x > y ? x : y;
}

#define MIN(X, Y) (_Generic((X) + (Y), \
size_t: min_size_t)((X), (Y)))

#define MAX(X, Y) (_Generic((X) + (Y), \
size_t: max_size_t)((X), (Y)))
