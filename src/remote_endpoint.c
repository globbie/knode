#include <string.h>

#include "remote_endpoint.h"

static void
event_cb(struct bufferevent *evbuf, short events, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;
    int error_code;

    (void) evbuf;

    fprintf(stderr, "debug3: RemoteEndPoint<%p> event occured\n", (void *) self);

    if (events & BEV_EVENT_CONNECTED) {
        fprintf(stderr, "debug3: RemoteEndPoint<%p> connected\n", (void *) self);
        error_code = self->event_cb(self, KMQ_EPEVENT_CONNECTED, self->cb_arg);
    } else if (events & BEV_EVENT_ERROR ||
               events & BEV_EVENT_WRITING ||
               events & BEV_EVENT_READING) {
        fprintf(stderr, "debug3: RemoteEndPoint<%p> error occured, error: '%s'\n", (void *) self, strerror(errno));
        error_code = self->event_cb(self, KMQ_EPEVENT_ERROR, self->cb_arg);
    } else if (events & BEV_EVENT_EOF) {
        fprintf(stderr, "debug3: RemoteEndPoint<%p> disconnected\n", (void *) self);
        error_code = self->event_cb(self, KMQ_EPEVENT_DISCONNECTED, self->cb_arg);
    }
}

static void
write_cb(struct bufferevent *evbuf, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;
    (void) evbuf;
    fprintf(stderr, "debug3: RemoteEndPoint<%p> write callback\n", (void *) self);
}

// todo: all errors should bring REP into error state
static void
read_cb(struct bufferevent *evbuf, void *arg)
{
    struct kmqRemoteEndPoint *self = arg;
    struct evbuffer *input;
    struct chunk_header *header;
    size_t total_chunk_size;
    size_t input_length;

    int error_code;

    fprintf(stderr, "debug3: RemoteEndPint<%p> read callback\n", (void *) self);

    /*
    input = bufferevent_get_input(evbuf);
    if (!input) return;

    input_length = evbuffer_get_length(input);

    if (input_length < sizeof(*header)) return;

    header = (struct chunk_header *) evbuffer_pullup(input, sizeof(*header));
    // todo: check if length fits size_t
    total_chunk_size = sizeof(*header) + header->length;

    if (input_length == sizeof(*header)) {
        if (header->length == 0) return;

        bufferevent_setwatermark(evbuf, EV_READ, total_chunk_size, total_chunk_size);
        error_code = evbuffer_expand(input, total_chunk_size);
        if (error_code != 0) return;
    }

    if (input_length == total_chunk_size) {
        header = (struct chunk_header *) evbuffer_pullup(input, total_chunk_size);

        // todo: callback invocation
        error_code = self->read_cb(self, header->payload, header->length, self->cb_arg);
        if (error_code != 0) return;

        error_code = evbuffer_drain(input, total_chunk_size);
        if (error_code != 0) return;
        bufferevent_setwatermark(evbuf, EV_READ, sizeof(*header), sizeof(*header));
    }
    */
}

static int
send_(struct kmqRemoteEndPoint *self, struct kmqTask *task)
{
    struct kmqChunkPipeline *pipeline;
    struct chunk *chunk, *save;
    struct evbuffer *output;
    int error_code;

    // this implementation violates incapsulation
    // todo: rewrite it

    /*
    pipeline = task->output;

    list_foreach_entry_safe(chunk, save, struct chunk, &pipeline->chunks, chunks_entry) {
        // todo: add header
        // todo: add payload
        // todo: remove chunk from pipeline
        // todo: free chunk
    }
    */

    /*
    if (buf_len == 0) return -1;

    memset(&header, 0, sizeof(header));
    header.length = buf_len;

    output = bufferevent_get_output(self->evbuf);
    if (!output) return -1;

    error_code = evbuffer_add(output, &header, sizeof(header));
    if (error_code != 0) return -1;

    error_code = evbuffer_add(output, buf, buf_len);
    if (error_code != 0) return -1;
    */

    return 0;
}

static int
set_address(struct kmqRemoteEndPoint *self, const struct addrinfo *address)
{
    self->options.address = address;
    return 0;
}

// note: accept_ and connect_ function are pretty much the same. todo: merge them
static int
accept_(struct kmqRemoteEndPoint *self, struct event_base *evbase, evutil_socket_t fd)
{
    int error_code;

    fprintf(stderr, "debug2: RemoteEndPoint<%p> acepted connection\n", (void *) self);

    self->evbuf = bufferevent_socket_new(evbase, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!self->evbuf) return -1;

    bufferevent_setcb(self->evbuf, read_cb, write_cb, event_cb, self);

    error_code = bufferevent_enable(self->evbuf, EV_READ | EV_WRITE);
    if (error_code != 0) goto error;

    /*
    bufferevent_setwatermark(self->evbuf, EV_READ,
            sizeof(struct chunk_header), sizeof(struct chunk_header));
    */
    return 0;
error:
    bufferevent_free(self->evbuf);
    return -1;
}

static int
connect_(struct kmqRemoteEndPoint *self, struct event_base *evbase)
{
    int error_code;

    fprintf(stderr, "debug2: RemoteEndPoint<%p> connecting...\n", (void *) self);

    self->evbuf = bufferevent_socket_new(evbase, -1, BEV_OPT_CLOSE_ON_FREE);
    if (!self->evbuf) return -1;

    error_code = bufferevent_socket_connect(self->evbuf,
            self->options.address->ai_addr, self->options.address->ai_addrlen);
    if (error_code != 0) goto error;

    bufferevent_setcb(self->evbuf, read_cb, write_cb, event_cb, self);

    error_code = bufferevent_enable(self->evbuf, EV_READ | EV_WRITE);
    if (error_code != 0) goto error;

    {
        int fd, opt = 1, os = sizeof(opt);

        fd = bufferevent_getfd(self->evbuf);
        if (fd == -1) goto error;

        error_code = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, os);
        if (error_code != 0) goto error;
    }

    // todo:
    //bufferevent_setwatermark(self->evbuf, EV_READ,
    //        sizeof(struct chunk_header), sizeof(struct chunk_header));

    return 0;
error:
    fprintf(stderr, "debug2: RemoteEndPoint<%p> connection failed\n", (void *) self);
    //bufferevent_free(self->evbuf);
    return 0;
}

static int
init(struct kmqRemoteEndPoint *self, struct event_base *evbase)
{
    (void) self; (void) evbase;
    return 0;
}

static int
delete(struct kmqRemoteEndPoint *self)
{
    if (self->evbuf) bufferevent_free(self->evbuf);
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
