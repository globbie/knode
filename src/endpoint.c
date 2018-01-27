#include "endpoint.h"

static int
heartbeat_cb(struct kmqTimer *timer, void *cb_arg)
{
    struct kmqEndPoint *self = cb_arg;
    struct kmqRemoteEndPoint *remote, *save;
    int error_code;

    (void) timer;

    fprintf(stderr, "debug3: EndPoint<%p> heartbeat\n", (void *) self);

    list_foreach_entry_safe(remote, save, struct kmqRemoteEndPoint,
                       &self->reconnect_remotes, endpoint_entry) {

        error_code = remote->connect(remote, self->evbase);
        if (error_code != 0) {
            fprintf(stderr, "EP<%p>: remote->connect() failed\n", (void *) self);
            continue;
        }
    }

    return 0;
}

static int
read_cb(struct kmqRemoteEndPoint *remote, struct kmqTask *task, void *cb_arg)
{
    struct kmqEndPoint *self = cb_arg;

    return self->options.callback(self, task);
}

static int
event_cb(struct kmqRemoteEndPoint *remote, enum kmqEndPointEvent event,
         void *cb_arg)
{
    struct kmqEndPoint *self = cb_arg;

    switch (event) {
    case KMQ_EPEVENT_CONNECTED:
        if (self->options.role == KMQ_INITIATOR) {
            list_move_tail(&self->remotes, &remote->endpoint_entry);
        }
        break;
    case KMQ_EPEVENT_ERROR:
        if (self->options.role == KMQ_INITIATOR) {
            list_move_tail(&self->reconnect_remotes, &remote->endpoint_entry);
        }
        break;
    case KMQ_EPEVENT_DISCONNECTED:
        break;
    default:
        fprintf(stderr, "error: RemoteEndPoint<%p> got unkown error from RemoteEndPoint<%p>\n",
                (void *) self, (void *) remote);
        return -1;
    }

    return 0;
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

    fprintf(stderr, "debug2: EndPoint<%p> connection accepted\n", (void *) self);
    return;
error:
    remote->del(remote);
}

static int
schedule_task(struct kmqEndPoint *self, struct kmqTask *task)
{
    int error_code = 0;

    switch (self->options.type) {
    case KMQ_PUB: {
        struct kmqRemoteEndPoint *remote;

        list_foreach_entry(remote, struct kmqRemoteEndPoint,
                &self->remotes, endpoint_entry) {

            error_code = remote->send(remote, task);
            if (error_code != 0) {
                fprintf(stderr, "remote->send() failed, error: %d\n", error_code);
                //return -1;
            }
        }

        break;
    }
    case KMQ_PUSH: {
        struct kmqRemoteEndPoint *remote;

        if (list_is_empty(&self->remotes)) return 0; // todo: add task to queue

        remote = list_first_entry(&self->remotes, struct kmqRemoteEndPoint, endpoint_entry);
        if (!remote) return -1; // todo: push to queue

        fprintf(stderr, "debug2: EndPoint<%p> sending task<%p> to RemoteEndPoint<%p>\n",
                (void *) self, (void *) task, (void *) remote);

        error_code = remote->send(remote, task);
        if (error_code != 0) {
            fprintf(stderr, "error: EndPoint<%p>->remote<%p>->send() failed, error: %d\n",
                    (void *) self, (void *) remote,  error_code);
        }

        list_move_tail(&self->remotes, &remote->endpoint_entry);
        break;
    }
    default:
        printf("not implemented yet.\n");
        return -1;
    }

    return 0;
}

/*
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
*/

static int
connect_(struct kmqEndPoint *self, struct event_base *evbase)
{
    struct kmqRemoteEndPoint *remote;
    int error_code = 0;

    fprintf(stderr, "debug2: EndPoint is trying to connect... \n");

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
        evconnlistener_new_bind(evbase, accept_cb, self,
                (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC |
                                                            LEV_OPT_REUSEABLE),
                -1,
                self->options.address->ai_addr,
                self->options.address->ai_addrlen);
    if (!self->listener) return -1;

    return 0;
}

static int
set_address(struct kmqEndPoint *self, const struct addrinfo *address)
{
    self->options.address = address;
    return 0;
}

static int
add_remote(struct kmqEndPoint *self, struct kmqRemoteEndPoint *remote)
{
    fprintf(stderr, "debug3: EndPoint<%p>->add_remote<%p>\n",
            (void *) self, (void *) remote);

    remote->read_cb = read_cb;
    remote->event_cb = event_cb;
    remote->cb_arg = self;

    list_add_tail(&self->remotes, &remote->endpoint_entry);
    return 0;
}

static int
init(struct kmqEndPoint *self, struct event_base *evbase)
{
    int error_code = -1;

    fprintf(stderr, "debug2: initializing EndPoint<%p>...\n", (void *) self);

    self->evbase = evbase;

    if (self->options.role == KMQ_TARGET) {
        error_code = bind_(self, evbase);
    } else if (self->options.role == KMQ_INITIATOR) {
        error_code = connect_(self, evbase);
    }

    error_code = self->heartbeat->init(self->heartbeat, evbase);

    return error_code;
}

static int
delete(struct kmqEndPoint *self)
{
    if (self->heartbeat) self->heartbeat->del(self->heartbeat);
    free(self);
    return 0;
}

int kmqEndPoint_new(struct kmqEndPoint **endpoint)
{
    struct kmqEndPoint *self;
    int error_code;

    self = calloc(1, sizeof(*self));
    if (!self) return -1;

    error_code = kmqTimer_new(&self->heartbeat);
    if (error_code != 0) goto error;

    // todo: fix magic number
    self->heartbeat->options.interval = (struct timeval) { .tv_sec = 10, .tv_usec = 0 };
    self->heartbeat->callback = heartbeat_cb;
    self->heartbeat->callback_arg = self;

    list_head_init(&self->remotes);
    list_head_init(&self->reconnect_remotes);

    self->schedule_task = schedule_task;
    self->set_address = set_address;
    self->add_remote = add_remote;
    self->init = init;
    self->del = delete;

    *endpoint = self;
    return 0;
error:
    delete(self);
    return -1;
}
