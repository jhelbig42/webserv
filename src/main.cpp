#include "Buffer.hpp"
#include "Logging.hpp"
#include <cstring>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#define FILE_MODE 0600
#define CHUNK_SIZE 128

int main(void) {
  Buffer buf;
  logging::log(logging::Info, "Buffer created");

  int fdin;
  int fdout;
  try {
    buf.fill(-1, CHUNK_SIZE);
    (void)buf.empty(STDOUT_FILENO, CHUNK_SIZE);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  try {
    fdin = open("Makefile", O_RDONLY);
    if (fdin < 0) {
      const int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    logging::log(logging::Info, "Makefile opened");
    buf.fill(fdin, CHUNK_SIZE);
    (void)buf.empty(STDOUT_FILENO, CHUNK_SIZE);
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  close(fdin);
  try {
    fdin = open("Makefile", O_RDONLY);
    if (fdin < 0) {
      const int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    fdout = open("test.txt", O_WRONLY | O_CREAT, FILE_MODE);
    if (fdout < 0) {
      close(fdin);
      const int err = errno;
      errno = 0;
      throw std::runtime_error(strerror(err));
    }
    while (1) {
      buf.fill(fdin, CHUNK_SIZE);
      if (buf.empty(fdout, CHUNK_SIZE) == 0 && buf.getUsed() == 0)
        break;
      // logging::log2(logging::Info, "_start:", buf.getBlocked());
      // logging::log2(logging::Info, "_end:", buf.getOccupied());
      buf.optimize(CHUNK_SIZE);
    }
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
  }
  close(fdin);
  close(fdout);
  buf.reset();
  std::cout << "\n> " << std::flush;
  buf.fill(STDIN_FILENO, CHUNK_SIZE);
  std::string s(buf.begin(), buf.end());
  logging::log(logging::Info, "string:");
    std::cout << s;
  logging::log(logging::Info, "indexing:");
  for (Buffer::size_type i = 0; i < buf.getUsed(); ++i)
    std::cout << buf[i];
}
