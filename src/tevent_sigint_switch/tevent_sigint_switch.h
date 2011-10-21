#ifndef TEVENT_SIGINT_SWITCH_H_
#define TEVENT_SIGINT_SWITCH_H_

#define EOK 0
#define DEBUG(msg) fprintf(stderr, "%s:%d - %s\n", __FUNCTION__, __LINE__, msg)

#define SIGINT_SWITCH_TIMEOUT   5

typedef int errno_t;

struct main_context
{
    struct tevent_context *event_ctx;
    int is_sigint_enabled;
};

static errno_t setup_event_handlers(struct main_context *main_ctx,
                                    struct tevent_context *event_ctx);

static void on_sigint(struct tevent_context *event_ctx,
                      struct tevent_signal *event_signal,
                      int signum,
                      int count,
                      void *siginfo,
                      void *private_data);

static void switch_sigint_enabled(struct tevent_context *ev,
                                  struct tevent_timer *te,
                                  struct timeval t,
                                  void *private_data);

#endif /* TEVENT_SIGINT_SWITCH_H_ */
