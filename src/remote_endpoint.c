#include "remote_endpoint.h"

static void
event_cb(struct bufferevent *evbuf, short events, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;

    if (events & BEV_EVENT_CONNECTED) {

    } else if (events * BEV_EVENT_ERROR) {

    } else if (events & BEV_EVENT_EOF) {

    }

    printf("event_cb\n");
}

static void
write_cb(struct bufferevent *evbuf, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;
    printf("write_cb\n");
}

static void
read_cb(struct bufferevent *evbuf, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;
    printf("read_cb\n");
}

static int
send_(struct kmqRemoteEndPoint *self, const char *buf, size_t buf_len)
{
    struct evbuffer *output;

    output = bufferevent_get_output(self->evbuf);
    evbuffer_add(output, buf, buf_len);

    return 0;
}

static int
connect_(struct kmqRemoteEndPoint *self, struct event_base *evbase)
{
    return 0;
}

static int
set_address(struct kmqRemoteEndPoint *self, const char *address, size_t address_len)
{
    return addrinfo_new(&self->options.address, address, address_len);
}

static int
accept_(struct kmqRemoteEndPoint *self, struct event_base *evbase, evutil_socket_t fd)
{
    self->evbuf = bufferevent_socket_new(evbase, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!self->evbuf) return -1;

    bufferevent_setcb(self->evbuf, read_cb, write_cb, event_cb, self);
    bufferevent_enable(self->evbuf, EV_READ | EV_WRITE);

    bufferevent_setwatermark(self->evbuf, EV_READ, 1, 1);

    return 0;
}

static int
init(struct kmqRemoteEndPoint *self, struct event_base *evbase)
{
    printf("[REP] init\n");
    // todo: this is conect only implementation
    // write accept connection implementation

    self->evbuf = bufferevent_socket_new(evbase, -1, BEV_OPT_CLOSE_ON_FREE);
    if (!self->evbuf) return -1;

    bufferevent_socket_connect(self->evbuf, self->options.address->ai_addr, self->options.address->ai_addrlen);

    bufferevent_setcb(self->evbuf, read_cb, write_cb, event_cb, self);
    bufferevent_enable(self->evbuf, EV_READ | EV_WRITE);

    // todo: do i need set keep-alive option?

    // todo: set watermark
    bufferevent_setwatermark(self->evbuf, EV_READ, 1, 1);

    return 0;
}

static int
delete(struct kmqRemoteEndPoint *self)
{
    free(self);
    return 0;
}

int
kmqRemoteEndPoint_new(struct kmqRemoteEndPoint **remote)
{
    struct kmqRemoteEndPoint *self;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    self->init = init;
    self->connect = connect_;
    self->accept = accept_;
    self->send = send_;
    self->set_address = set_address;
    self->del = delete;

    *remote = self;
    return 0;
//error:
//    delete(self);
//    return -1;
}
