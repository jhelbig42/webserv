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

  int fd;
  try {
    buf.fill(-1, 128);
    buf.empty(STDOUT_FILENO, 128);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  try {
    fd = open("Makefile", O_RDONLY);
    if (fd < 0) {
      int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    buf.fill(fd, 128);
    buf.empty(STDOUT_FILENO, 128);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
}
