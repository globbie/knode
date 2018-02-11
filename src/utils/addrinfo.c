#include <stdio.h>
#include <string.h>

#include <addrinfo.h>

#define NETWORK_ADDRESS_MAX_LEN 256

int
addrinfo_new(struct addrinfo **addrinfo,
             const char *address, size_t address_len)
{
    char network_address[NETWORK_ADDRESS_MAX_LEN];
    char *port;

    struct addrinfo hints;
    struct addrinfo *resolve;

    int error_code;

    if (address_len >= NETWORK_ADDRESS_MAX_LEN) return -1;

    memcpy(network_address, address, address_len);
    network_address[address_len] = '\0';

    port = strrchr(network_address, ':');
    if (port) {
        *port = '\0';
        ++port;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    error_code = getaddrinfo(network_address, port, &hints, &resolve);
    if (error_code != 0) {
        fprintf(stderr, "getaddinfo: %s", gai_strerror(error_code));
        return -1;
    }

    *addrinfo = resolve;
    return 0;
}

int
addrinfo_delete(struct addrinfo *addrinfo)
{
    freeaddrinfo(addrinfo);
    return 0;
}
