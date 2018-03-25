#include "knode-cat.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <event.h>

static void
read_handler(struct kmqKnodeCat *self, int fd)
{
    char input[STDIN_BUFFER_SIZE];
    ssize_t input_len;

    int error_code;

    input_len = read(fd, input, sizeof(input));
    if (input_len < 0) {
        fprintf(stderr, "error: kmqKnodeCat read failed, error: '%s'\n", strerror(errno));
        goto error;
    }

    if (input_len == 0) {
        fprintf(stderr, "debug2: kmqKnodeCat end of file\n");
        event_del(self->stdio_event);

        if (!self->current_task) return;

        fprintf(stderr, "debug3: kmqKnodeCat scheduling task (eof)\n");
        error_code = self->endpoint->schedule_task(self->endpoint, self->current_task);
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat endpoint failed to schedule task\n");
            goto error;
        }
        self->current_task = NULL;

        self->stop(self);
    }

    char *input_start = input;

    while (input_len > 0) { // todo: use for loop
        struct kmqTask *task = self->current_task;
        char *new_line = memchr(input_start, '\n', input_len);
        size_t buffer_size;

        if (!new_line) {
            if (!task) {
                error_code = self->get_task(self, &task);
                if (error_code != 0) goto error;
            }

            fprintf(stderr, "debug3: kmqKnodeCat<%p> add all to task\n", (void *) self);
            error_code = task->copy_data(task, input_start, input_len);
            if (error_code != 0) goto error;
            return;
        }

        buffer_size = new_line - input_start;

        if (buffer_size == 0) {
            if (!task) goto next;

            fprintf(stderr, "debug3: kmqKnodeCat scheduling task (new line only)\n");

            error_code = self->endpoint->schedule_task(self->endpoint, task);
            if (error_code != 0) {
                fprintf(stderr, "error: kmqKnodeCat knode failed to schedule task\n");
                goto error;
            }
            self->current_task = NULL;
            goto next;
        }

        if (!task) {
            error_code = self->get_task(self, &task);
            if (error_code != 0) goto error;
        }

        error_code = task->copy_data(task, input_start, buffer_size);
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat append data into task failed\n");
            goto error;
        }

        fprintf(stderr, "debug3: kmqKnodeCat scheduling task (new line)\n");
        error_code = self->endpoint->schedule_task(self->endpoint, self->current_task);
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat knode failed to schedule task\n");
            goto error;
        }
        self->current_task = NULL;

next:
        input_start += buffer_size + 1;
        input_len -= buffer_size + 1;
    }

    return;
error:
    self->stop(self);
}

// fixme: todo: warning: to send the reply we MUST know the remote endpoint to send reply
int task_callback(struct kmqEndPoint *endpoint, struct kmqTask *task, void *cb_arg)
{
    int error_code;
    (void) endpoint;
    (void) cb_arg;

    for (size_t i = 0; i < task->sg_items_count; ++i) {
        const char *data;
        size_t size;

        error_code = task->get_data(task, i,  &data, &size);
        if (error_code != 0) return -1;

        fprintf(stderr, "debug3: got task, size: %zu\n", size);
        printf("%.*s\n", (int) size, data);
    }

    // send reply
    if (endpoint->options.type == KMQ_PULL) {
        struct kmqTask *reply;

        error_code = kmqTask_new(&reply);
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat task allocation failed\n");
            return error_code;
        }

        error_code = task->copy_data(reply, "OK", sizeof("OK"));
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat append data into reply failed\n");
            goto free_reply;
        }
        error_code = endpoint->schedule_task(endpoint, reply);
        if (error_code != 0) {
            fprintf(stderr, "error: kmqKnodeCat schedule reply failed\n");
            goto free_reply;
        }

    free_reply:
        reply->del(reply);
    }

    return 0;
}

static void
event_handler(int fd, short event, void *arg)
{
    struct kmqKnodeCat *self = arg;

    if (event & EV_READ) {
        fprintf(stderr, "debug2: kmqKnodeCat read event\n");
        read_handler(self, fd);
    }
}

static int
stop__(struct kmqKnodeCat *self)
{
    return self->knode->stop(self->knode);
}

static int
get_task__(struct kmqKnodeCat *self, struct kmqTask **task)
{
    int error_code;

    if (self->current_task) {
        *task = self->current_task;
        return 0;
    }

    error_code = kmqTask_new(&self->current_task);
    if (error_code != 0) {
        fprintf(stderr, "error: kmqKnodeCat task allocation failed\n");
        return error_code;
    }

    *task = self->current_task;

    return 0;

}

static int
start__(struct kmqKnodeCat *self)
{
    int error_code = -1;

    error_code = self->knode->dispatch(self->knode);

    return error_code;
}

static int
del__(struct kmqKnodeCat *self)
{
    if (self->remote) self->remote->del(self->remote);
    if (self->endpoint) self->endpoint->del(self->endpoint);
    if (self->knode) self->knode->del(self->knode);

    free(self);

    return 0;
}

int
kmqKnodeCat_new(struct kmqKnodeCat **service, const struct kmqKnodeCatConfig *config)
{
    struct kmqKnodeCat *self = calloc(1, sizeof(*self));
    int error_code;
    if (!self) return -1;

    self->config = config;

    error_code = kmqKnode_new(&self->knode);
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&self->endpoint);
    if (error_code != 0) goto error;

    self->endpoint->options.type = self->config->endpoint_type;

    if (self->config->endpoint_role == KMQ_INITIATOR) {
        self->endpoint->options.role = KMQ_INITIATOR;

        error_code = kmqRemoteEndPoint_new(&self->remote);
        if (error_code != 0) goto error;

        error_code = self->remote->set_address(self->remote, self->config->address);
        if (error_code != 0) goto error;

        error_code = self->endpoint->add_remote(self->endpoint, self->remote);
        if (error_code != 0) goto error;

        self->endpoint->options.callback = task_callback;

    } else {
        self->endpoint->options.role = KMQ_TARGET;

        error_code = self->endpoint->set_address(self->endpoint, self->config->address);
        if (error_code != 0) goto error;

        self->endpoint->options.callback = task_callback;
    }

    error_code = self->knode->add_endpoint(self->knode, self->endpoint);
    if (error_code != 0) goto error;

    if (self->endpoint->options.type == KMQ_PUSH) {
        self->stdio_event = event_new(self->knode->evbase, 0, EV_READ | EV_PERSIST, event_handler, self);
        event_add(self->stdio_event, NULL);
    }

    self->del = del__;
    self->start = start__;
    self->stop = stop__;
    self->get_task = get_task__;

    *service = self;
    return 0;
error:
    del__(self);
    return error_code;
}
