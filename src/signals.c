#include <csignal>

static volatile sig_atomic_t g_sig_val = 0;

static void handler(int Signum) {
  g_sig_val = Signum;
}

int checkSignal(void) {
  return g_sig_val;
}

int registerSigint(void) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sa, NULL) < 0)
    return -1;
  return 0;
}
