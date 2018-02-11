#pragma once

#include <netdb.h>

int addrinfo_new(struct addrinfo **addrinfo,
                 const char *address, size_t address_len);
int addrinfo_delete(struct addrinfo *addrinfo);
