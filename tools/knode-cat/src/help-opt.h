#pragma once

#include <stdlib.h>

#include "option.h"

extern struct glbOptType kndHelpOptType;

int kndHelpOptType_parse(struct glbOption *self, const char *input, size_t input_len);
int kndHelpOptType_free(struct glbOption *self);
