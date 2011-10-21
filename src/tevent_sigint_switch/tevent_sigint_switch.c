#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <talloc.h>
#include <tevent.h>

#include "tevent_sigint_switch.h"

int main()
{
    struct main_context *ctx = NULL;
    int ret;

    /* Create main context */
    ctx = talloc_zero(NULL, struct main_context);
    if (ctx == NULL) {
        DEBUG("talloc_zero() failed");
        goto fail;
    }
    ctx->is_sigint_enabled = 0;

    /* Create main event context */
    ctx->event_ctx = tevent_context_init(ctx);
    if (ctx->event_ctx == NULL) {
        DEBUG("talloc_context_init() failed");
        goto fail;
    }

    /* Setup event handlers on event context */
    ret = setup_event_handlers(ctx, ctx->event_ctx);
    if (ret != EOK) {
        DEBUG("setup_event_handlers() failed");
        goto fail;
    }

    /* Enter event loop */
    tevent_loop_wait(ctx->event_ctx);

    ret = EXIT_SUCCESS;
    goto done;
fail:
    ret = EXIT_FAILURE;

done:
    talloc_free(ctx);

	return ret;
}

errno_t setup_event_handlers(struct main_context *main_ctx,
                             struct tevent_context *event_ctx)
{
    struct tevent_signal *event_signal = NULL;
    struct tevent_timer *event_timer = NULL;
    struct timeval tv;

    event_signal = tevent_add_signal(event_ctx, event_ctx, SIGINT,
                                     0, on_sigint, main_ctx);
    if (event_signal == NULL) {
        DEBUG("tevent_add_signal() failed");
        return EIO;
    }

    tv = tevent_timeval_current_ofs(SIGINT_SWITCH_TIMEOUT, 0);
    event_timer = tevent_add_timer(event_ctx, event_ctx, tv,
                                   switch_sigint_enabled, main_ctx);
    if (event_timer == NULL) {
        DEBUG("tevent_add_signal() failed");
        return EIO;
    }

    return EOK;
}

void on_sigint(struct tevent_context *event_ctx,
               struct tevent_signal *event_signal,
               int signum,
               int count,
               void *siginfo,
               void *private_data)
{
    DEBUG("SIGINT received");
    struct main_context *main_ctx = talloc_get_type(private_data,
                                                    struct main_context);
    if (main_ctx->is_sigint_enabled) {
        DEBUG("Exiting...");
        kill(getpid(), SIGTERM);
    }
}

void switch_sigint_enabled(struct tevent_context *ev,
                           struct tevent_timer *te,
                           struct timeval t,
                           void *private_data)
{
    struct timeval tv;
    struct tevent_timer *event_timer = NULL;
    struct main_context *main_ctx = talloc_get_type(private_data,
                                                    struct main_context);

    if (main_ctx->is_sigint_enabled) {
        main_ctx->is_sigint_enabled = 0;
        DEBUG("SIGINT disabled");
    } else {
        main_ctx->is_sigint_enabled = 1;
        DEBUG("SIGINT enabled");
    }

    tv = tevent_timeval_current_ofs(SIGINT_SWITCH_TIMEOUT, 0);
    event_timer = tevent_add_timer(main_ctx->event_ctx, main_ctx->event_ctx, tv,
                                   switch_sigint_enabled, main_ctx);
    if (event_timer == NULL) {
        DEBUG("tevent_add_signal() failed");
    }
}
