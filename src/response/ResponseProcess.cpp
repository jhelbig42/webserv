#include "Response.hpp"

#include "Buffer.hpp"
#include "Logging.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>

// TODO: Compare data sent with header Content-Length instead of only relying on
// read() return values

/// \brief reads data from a file descriptor and writes it to a socket
///
/// A buffer is used to save the state of the transmission between calls.
/// The buffer should not be changed between calls until the transmission
/// is done.
///
/// side effects:
/// Reads from FileFd
/// Writes to Socket
/// modifies Buf
/// If there is nothing more to read from FileFd closes it and sets it to -1
/// Logs and error if closing FileFd fails
///
/// \param Socket socket to send data to
/// \param FileFd file to read data from, will be set to -1 if nothing more to
/// read \param Buf buffer which is used to save the transmission state between
/// calls \param Bytes amount of bytes which is attempted to be transmitted
///
/// \returns true if transmission is completed
/// \returns false if transmission is not completed
static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf,
                         const size_t Bytes);

/// \brief writes the content of an std::string object to a socket
///
/// side effects:
/// if the entire string Str can't be sent
/// it will be modified to represent the unsent data
/// Writes to Socket
///
/// \returns true if transmission is completed
/// \returns false if transmission is not completed
static bool stringToSocket(const int Socket, std::string &Str,
                           const size_t Bytes);

bool Response::process(const int Socket, const size_t Bytes) {
  if (_ptype == SendFile)
    return sendFile(Socket, Bytes);
  throw std::runtime_error("Unknown process type");
}

bool Response::sendFile(const int Socket, const size_t Bytes) {
  if (!_metadataSent) {
    _metadataSent = stringToSocket(Socket, _metadata, Bytes);
    return false;
  }
  return fileToSocket(Socket, _fdIn, _buffer, Bytes);
}

// TODO: catching SIGPIPE still missing
// TODO: handle rc < 0
// TODO: doublecheck error handling
static bool stringToSocket(const int Socket, std::string &Str,
                           const size_t Bytes) {
  if (Str.empty())
    return true;
  const size_t amount = std::min(Bytes, Str.size());
  const ssize_t rc = write(Socket, Str.c_str(), amount);
  if (rc < 0) {
    logging::log3(logging::Error, __func__, ": ", strerror(errno));
    return false;
  }
  if ((size_t)rc == amount) // cast is safe because rc > 0
    return true;
  Str.erase(0, (size_t)rc); // cast is safe because rc > 0
  return false;
}

// for ideas about performance improvement check comments to implementations
// of Buffer::fill()
static bool fileToSocket(const int Socket, int &FileFd, Buffer &Buf,
                         const size_t Bytes) {
  if (FileFd >= 0) {
    Buf.optimize(Bytes);
    // edgecase = FileFd empties by exactly filling up Buf. Then empty FileFd
    // would be passed one more time; theory: does not matter as FileFd will
    // spill just 0 next iteration and no exception will be thrown
    const ssize_t rc = Buf.fill(FileFd, Bytes);
    if ((size_t)rc < Bytes && Buf.getFree() > 0) {
      if (close(FileFd) < 0) {
        logging::log2(logging::Error, "close: ", strerror(errno));
        errno = 0;
      }
      FileFd = -1;
    }
  }
  const size_t used = Buf.getUsed();
  if (used == 0) // nothing read from FileFd and no residue from last time
    return true;
  const ssize_t rc = Buf.empty(Socket, Bytes);
  if (FileFd == -1 && ((rc >= 0) && (size_t)rc == used))
    return true;
  return false;
}
