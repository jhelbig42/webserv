#include "Buffer.hpp"
#include "Logging.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <exception>
#include <unistd.h>

#define BYTES_PER_CHUNK 256

int main(void) {
  Request const req(Get, "/home/alneuman/projects/webserv/Makefile", 1, 0, true);
  Response res(req);
  try {
    while (!res.process(STDOUT_FILENO, BYTES_PER_CHUNK))
      logging::log(logging::Error, "hi");
  } catch (std::exception &e) {
    logging::log(logging::Error, e.what());
    return 1;
  }
  return 0;
}

/*
#include <cstring>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#define FILE_MODE 0600
#define CHUNK_SIZE 128
#define DELETE_FRONT 3

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
    buf.fill(fdin, CHUNK_SIZE);
    while (1) {
      if (buf.empty(fdout, CHUNK_SIZE) < 0 && buf.fill(fdin, CHUNK_SIZE) == 0)
        break;
      logging::log2(logging::Info, "_start:", buf.getBlocked());
      logging::log2(logging::Info, "_end:", buf.getOccupied());
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
  const std::string s(buf.begin(), buf.end());
  logging::log(logging::Info, "string:");
  std::cout << s;
  logging::log(logging::Info, "indexing:");
  for (size_t i = 0; i < buf.getUsed(); ++i)
    std::cout << buf[i];
  buf.deleteFront(DELETE_FRONT);
  logging::log3(logging::Info, "first ", DELETE_FRONT, " characters deleted:");
  for (size_t i = 0; i < buf.getUsed(); ++i)
    std::cout << buf[i];
  return 0;
>>>>>>> refs/remotes/origin/response_initialization
}
*/
