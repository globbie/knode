#include <stdlib.h>
#include <stdio.h>

#include <kmq.h>

static int
timer_subscribber_callback(struct kmqEndPoint *timer_subscriber, const char *buf, size_t buf_len)
{
    printf("%.*s\n", (int) buf_len, buf);
    return 0;
}

int main(int argc, const char **argv)
{
    struct kmqKnode *knode;
    struct kmqEndPoint *timer_subscriber;
    struct kmqRemoteEndPoint *remote_timer;

    const char *remote_timer_address = "127.0.0.1:8081";

    int error_code;
    int exit_code = EXIT_FAILURE;

    error_code = kmqKnode_new(&knode);
    if (error_code != 0) goto error;

    error_code = kmqRemoteEndPoint_new(&remote_timer);
    if (error_code != 0) goto error;

    error_code = remote_timer->set_address(remote_timer,
                                           remote_timer_address,
                                           strlen(remote_timer_address));
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&timer_subscriber);
    if (error_code != 0) goto error;

    timer_subscriber->options.type = KMQ_SUB;
    timer_subscriber->options.role = KMQ_INITIATOR;
    timer_subscriber->options.reliability = KMQ_ACK_OFF;
    timer_subscriber->callback = timer_subscriber_callback;

    error_code = timer_subscriber->init(timer_subscriber);
    if (error_code != 0) goto error;

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
    if (knode) knode->del(knode);

    return exit_code;
}
