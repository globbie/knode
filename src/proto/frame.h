#pragma once

#include <stdint.h>

struct kmq_locomotive_frame
{
    uint16_t type;
    uint16_t flags;
    uint32_t task_id;
    uint32_t carriage_size;
    uint32_t carriages_count;
} __attribute__((packed));
_Static_assert(sizeof(struct kmq_locomotive_frame) == 16, "wrong size of locomotive frame");

struct kmq_carriage_frame
{
    uint16_t type;
    uint32_t number;
    uint16_t reserved;
} __attribute__((packed));
_Static_assert(sizeof(struct kmq_carriage_frame) == 8, "wrong size of carriage frame");

