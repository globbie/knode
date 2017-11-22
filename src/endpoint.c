#include "endpoint.h"

static int
send_(struct kmqEndPoint *self, const char *buf, size_t buf_len)
{
    (void) self; (void) buf; (void) buf_len;
    return 0;
}

static void
accept_cb(struct evconnlistener *listener, evutil_socket_t fd,
          struct sockaddr *addr, int len, void *arg)
{
    struct kmqEndPoint *self = arg; (void) self;
    (void) listener; (void) fd; (void) addr; (void) len;
}

static int
set_address(struct kmqEndPoint *self, const char *address, size_t address_len)
{
    int error_code;

    error_code = addrinfo_new(&self->options.address, address, address_len);

    return error_code;
}

static int
add_remote(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote)
{
    (void) self; (void) remote;
    return 0;
}

static int
init(struct kmqEndPoint *self)
{
    (void) self;
    return 0;
}

static int
delete(struct kmqEndPoint *self)
{
    if (self->options.address) addrinfo_delete(self->options.address);
    free(self);
    return 0;
}

int kmqEndPoint_new(struct kmqEndPoint **endpoint)
{
    struct kmqEndPoint *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->accept_cb = accept_cb;

    self->send = send_;
    self->set_address = set_address;
    self->add_remote = add_remote;
    self->init = init;
    self->del = delete;

    *endpoint = self;
    return 0;
//error:
//    delete(self);
//    return -1;
}
