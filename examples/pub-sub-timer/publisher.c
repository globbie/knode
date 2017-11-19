#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kmq.h>

static int
timer_tick_cb(struct kmqTimer *self)
{
    // todo:
    return -1;
}

int main(int argc, const char **argv)
{
    struct kmqKnode *knode;
    struct kmqEndPoint *timer_publisher;
    struct kmqTimer *timer;

    const char *local_address = "127.0.0.1:8081";

    int error_code;
    int exit_code = EXIT_FAILURE;


    error_code = kmqKnode_new(&knode);
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&timer_publisher);
    if (error_code != 0) goto error;

    error_code = timer_publisher->set_address(timer_publisher,
                                              local_address, strlen(local_address));
    if (error_code != 0) goto error;
    timer_publisher->options.type = KMQ_SUB;
    timer_publisher->options.role = KMQ_INITIATOR;
    timer_publisher->options.reliability = KMQ_ACK_OFF;

    error_code = timer_publisher->init(timer_publisher);
    if (error_code != 0) goto error;

    error_code = kmqTimer_new(&timer);
    if (error_code != 0) goto error;

    timer->options.tick = 5; // todo: make proper interface
    timer->callback = timer_tick_cb;

    error_code = timer->init(timer);
    if (error_code != 0) goto error;

    error_code = knode->add_endpoint(knode, timer_publisher);
    if (error_code != 0) goto error;

    error_code = knode->add_timer(knode, timer);

    error_code = knode->dispatch(knode);
    if (error_code != 0) goto error;

    exit_code = EXIT_SUCCESS;
error:
    if (timer) timer->del(timer);
    if (timer_publisher) timer_publisher->del(timer_publisher);
    if (knode) knode->del(knode);

    return exit_code;
}
