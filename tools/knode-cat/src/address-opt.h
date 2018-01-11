#pragma once

#include <stdlib.h>

#include "option.h"

extern struct glbOptType kndAddressOptType;

int kndAddressOpt_parse(struct glbOption *self, const char *input, size_t input_len);
int kndAddressOpt_free(struct glbOption *self);
