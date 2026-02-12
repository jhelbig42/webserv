#include "Buffer.hpp"
#include "Logging.hpp"
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

int main(void) {
  Buffer buf;
  logging::log(logging::Info, "Buffer created");

  int fdin;
  int fdout;
  try {
    buf.fill(-1, 128);
    buf.empty(STDOUT_FILENO, 128);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  try {
    fdin = open("Makefile", O_RDONLY);
    if (fdin < 0) {
      int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    logging::log(logging::Info, "Makefile opened");
    buf.fill(fdin, 128);
    buf.empty(STDOUT_FILENO, 128);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  close(fdin);
  try {
    fdin = open("Makefile", O_RDONLY);
    if (fdin < 0) {
      int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    fdout = open("test.txt", O_WRONLY | O_CREAT, 0600);
    if (fdout < 0) {
      close(fdin);
      int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    while (1)
    {
      buf.fill(fdin, 128);
      if (buf.empty(fdout, 128) == 0 && buf.getUsed() == 0)
        break;
      logging::log2(logging::Info, "_start:", buf.getBlocked());
      logging::log2(logging::Info, "_end:", buf.getOccupied());
      buf.optimize(128);
    }
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  close(fdin);
  close(fdout);
}
