#pragma once

// note: this is temporary implementation

#include <stdint.h>

struct chunk_header {
    uint64_t length;
    char payload[];
};
