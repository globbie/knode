#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kmq.h>

/*
static int
timer_subscriber_cb(struct kmqEndPoint *timer_subscriber,
                          const char *buf, size_t buf_len)
{
    (void) timer_subscriber;
    printf("got message: '%.*s'\n", (int) buf_len, buf);
    return 0;
}
*/

static int
timer_subscriber_cb(struct kmqEndPoint *timer_subscriber, struct kmqTask *task, void *cb_arg)
{
    (void) timer_subscriber;
    (void) task;
    (void) cb_arg;
    return 0;
}

int main(int argc __attribute__((unused)),
         const char **argv __attribute__((unused)))
{
    struct kmqKnode *knode = NULL;
    struct kmqEndPoint *timer_subscriber = NULL;
    struct kmqRemoteEndPoint *remote_timer = NULL;

    const char *remote_timer_address = "127.0.0.1:8081";
    struct addrinfo *address = NULL;

    int error_code;
    int exit_code = EXIT_FAILURE;

    error_code = kmqKnode_new(&knode);
    if (error_code != 0) goto error;

    error_code = kmqRemoteEndPoint_new(&remote_timer);
    if (error_code != 0) goto error;

    error_code = addrinfo_new(&address, remote_timer_address, strlen(remote_timer_address));
    if (error_code != 0) goto error;

    error_code = remote_timer->set_address(remote_timer, address);
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&timer_subscriber);
    if (error_code != 0) goto error;

    timer_subscriber->options.type = KMQ_SUB;
    timer_subscriber->options.role = KMQ_INITIATOR;
    timer_subscriber->options.reliability = KMQ_ACK_OFF;
    timer_subscriber->options.callback = timer_subscriber_cb;

    error_code = timer_subscriber->add_remote(timer_subscriber, remote_timer);
    if (error_code != 0) goto error;

    error_code = knode->add_endpoint(knode, timer_subscriber);
    if (error_code != 0) goto error;

    error_code = knode->dispatch(knode);
    if (error_code != 0) goto error;

    exit_code = EXIT_SUCCESS;
error:
    if (timer_subscriber) timer_subscriber->del(timer_subscriber);
    if (remote_timer) remote_timer->del(remote_timer);
    if (address) addrinfo_delete(address);
    if (knode) knode->del(knode);

    return exit_code;
}
