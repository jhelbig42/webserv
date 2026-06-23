#include <cerrno>
#include <csignal>

static void terminateHandler(int Signum);

static volatile sig_atomic_t terminate = 0;

bool receivedTerminationSignal(void) { return terminate; }

int registerSignals(void) {
  struct sigaction sa;
  sa.sa_handler = terminateHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sa, NULL) < 0)
    return errno;
  if (sigaction(SIGTERM, &sa, NULL) < 0)
    return errno;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;

  if (sigaction(SIGPIPE, &sa, NULL) < 0)
    return errno;

  return 0;
}

static void terminateHandler(int Signum) {
  (void)Signum;
  terminate = 1;
}
