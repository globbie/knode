#pragma once

#include "option.h"
#include "address-opt.h"
#include "help-opt.h"

#define GLB_OPTS_HELP { "help", 'h', "Show help message", false, NULL, &kndHelpOptType }
#define GLB_OPTS_TERMINATOR { NULL, 0, NULL, false, NULL, NULL }

#define GLB_OPTS_COUNT_MAX 512
