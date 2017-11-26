#include "endpoint.h"


static int
read_cb(struct kmqRemoteEndPoint *remote, const char *buf, size_t len,
        void *cb_arg)
{
    struct kmqEndPoint *self = cb_arg; (void) remote;
    return self->options.callback(self, buf, len);
}

static void
accept_cb(struct evconnlistener *listener, evutil_socket_t fd,
          struct sockaddr *addr, int len, void *arg)
{
    struct kmqEndPoint *self = arg;
    struct kmqRemoteEndPoint *remote;

    int error_code;


    (void) addr; (void) len;

    error_code = kmqRemoteEndPoint_new(&remote);
    if (error_code != 0) return;

    error_code = remote->accept(remote, evconnlistener_get_base(listener), fd);
    if (error_code != 0) goto error;

    error_code = self->add_remote(self, remote);
    if (error_code != 0) goto error;

    printf("connection accepted\n");
    return;
error:
    remote->del(remote);
}

static int
send_(struct kmqEndPoint *self, const char *buf, size_t buf_len)
{
    struct kmqRemoteEndPoint *remote;
    int error_code = 0;

    if (self->options.type == KMQ_PUB) {

        list_foreach_entry(remote, struct kmqRemoteEndPoint, &self->remotes,
                           endpoint_entry) {
            error_code = remote->send(remote, buf, buf_len);
            if (error_code != 0) fprintf(stderr, "remote->send() failed,"
                                                 "error: %d\n", error_code);
        }

    } else {
        printf("not implemented yet\n");
        return -1;
    }

    return 0;
}

static int
connect_(struct kmqEndPoint *self, struct event_base *evbase)
{
    struct kmqRemoteEndPoint *remote;
    int error_code = 0;

    printf("endpoint is trying to connect... \n");

    list_foreach_entry(remote, struct kmqRemoteEndPoint,
                       &self->remotes, endpoint_entry) {

        error_code = remote->connect(remote, evbase);
        if (error_code != 0) fprintf(stderr, "remote->connect() failed,"
                                             " error: %d\n", error_code);
    }

    return 0;
}

static int
bind_(struct kmqEndPoint *self, struct event_base *evbase)
{
    self->listener = \
        evconnlistener_new_bind(evbase, accept_cb,self,
                (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC |
                                                            LEV_OPT_REUSEABLE),
                -1,
                self->options.address->ai_addr,
                self->options.address->ai_addrlen);

    if (!self->listener) return -1;
    return 0;
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
    remote->read_cb = read_cb;
    remote->cb_arg = self;

    list_add_tail(&self->remotes, &remote->endpoint_entry);
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

    list_head_init(&self->remotes);

    self->send = send_;
    self->connect = connect_;
    self->bind = bind_;
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
