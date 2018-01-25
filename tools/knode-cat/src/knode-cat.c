#include "knode-cat.h"

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
        fprintf(stderr, "error: read failed, error: '%s'\n", strerror(errno));
        goto error;
    }

    if (input_len == 0) {
        fprintf(stderr, "debug2: end of file\n");
        event_del(self->stdio_event);

        if (!self->current_task) return;

        fprintf(stderr, "debug3: scheduling task (eof)\n");
        error_code = self->endpoint->schedule_task(self->endpoint, self->current_task);
        if (error_code != 0) {
            fprintf(stderr, "error: endpoint failed to schedule task\n");
            goto error;
        }
        self->current_task = NULL;
    }

    char *input_start = input;

    while (input_len > 0) {
        struct kmqTask *task = self->current_task;
        char *new_line = memchr(input_start, '\n', input_len);
        size_t buffer_size;

        if (!new_line) {
            if (!task) {
                error_code = self->get_task(self, &task);
                if (error_code != 0) goto error;
            }

            fprintf(stderr, "debug3: add all to task\n");
            error_code = task->add_data_copy(task, input_start, input_len);
            if (error_code != 0) goto error;
            return;
        }

        buffer_size = new_line - input_start;

        if (buffer_size == 0) {
            if (!task) goto next;

            fprintf(stderr, "debug3: scheduling task (new line only)\n");

            error_code = self->endpoint->schedule_task(self->endpoint, task);
            if (error_code != 0) {
                fprintf(stderr, "error: knode failed to schedule task\n");
                goto error;
            }
            self->current_task = NULL;
            goto next;
        }

        if (!task) {
            error_code = self->get_task(self, &task);
            if (error_code != 0) goto error;
        }

        error_code = task->add_data_copy(task, input_start, buffer_size);
        if (error_code != 0) {
            fprintf(stderr, "error: append data into task failed\n");
            goto error;
        }

        fprintf(stderr, "debug3: scheduling task (new line)\n");
        error_code = self->endpoint->schedule_task(self->endpoint, self->current_task);
        if (error_code != 0) {
            fprintf(stderr, "error: knode failed to schedule task\n");
            goto error;
        }
        self->current_task = NULL;

next:
        input_start += buffer_size + 1;
        input_len -= buffer_size + 1;
    }

error:
    self->stop(self);
}

static void
event_handler(int fd, short event, void *arg)
{
    struct kmqKnodeCat *self = arg;

    if (event & EV_READ) {
        fprintf(stderr, "debug2: read event\n");
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
    if (error_code != 0) return error_code;

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

    } else {
        self->endpoint->options.role = KMQ_TARGET;

        error_code = self->endpoint->set_address(self->endpoint, self->config->address);
        if (error_code != 0) goto error;
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
