#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <kmq.h>

struct TimerContext
{
    struct kmqEndPoint *publisher;
    uint8_t ticks_counter;
};

static int
timer_tick_cb(struct kmqTimer *self, void *cb_arg)
{
    struct TimerContext *timer_ctx = cb_arg;
    char ticks_str[4] = { '\0' }; // todo: fix magic
    int error_code;

    (void) self;

    ++timer_ctx->ticks_counter;

    snprintf(ticks_str, sizeof(ticks_str) - 1,
             "%"PRIu8, timer_ctx->ticks_counter);

    printf("timer tick: '%s'\n", ticks_str);

    error_code = timer_ctx->publisher->send(timer_ctx->publisher,
                                           ticks_str, sizeof(ticks_str));

    return error_code;
}

int main(int argc __attribute__((unused)),
         const char **argv __attribute__((unused)))
{
    struct kmqKnode *knode = NULL;
    struct kmqEndPoint *timer_publisher = NULL;
    struct kmqTimer *timer = NULL;

    struct TimerContext timer_ctx;

    const char *local_address = "127.0.0.1:8081";

    int error_code;
    int exit_code = EXIT_FAILURE;

    error_code = kmqKnode_new(&knode);
    if (error_code != 0) goto error;

    error_code = kmqEndPoint_new(&timer_publisher);
    if (error_code != 0) goto error;

    error_code = timer_publisher->set_address(timer_publisher,
                                              local_address,
                                              strlen(local_address));
    if (error_code != 0) goto error;
    timer_publisher->options.type = KMQ_SUB;
    timer_publisher->options.role = KMQ_INITIATOR;
    timer_publisher->options.reliability = KMQ_ACK_OFF;

    error_code = timer_publisher->init(timer_publisher);
    if (error_code != 0) goto error;

    error_code = kmqTimer_new(&timer);
    if (error_code != 0) goto error;

    timer_ctx = (struct TimerContext) {
                    .ticks_counter = 0,
                    .publisher = timer_publisher
                };

    timer->options.interval = (struct timeval) { .tv_sec = 5, .tv_usec = 0 };
    timer->callback = timer_tick_cb;
    timer->callback_arg = &timer_ctx;

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
