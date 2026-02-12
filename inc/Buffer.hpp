#pragma once

#include "CompileTimeConstants.hpp"

class Buffer {
public:
  Buffer(void);
  Buffer(const Buffer &other);
  Buffer &operator=(const Buffer &other);
  ~Buffer(void);

  const static size_t size = BUFFER_SIZE;

  /// \brief get currently occupied storage in Buffer
  size_t getOccupied(void) const;

  /// \brief gets currently meaningfully occupied storage in Buffer
  size_t getUsed(void) const;

  /// \brief gets currently free storage in Buffer
  size_t getFree(void) const;

  /// \brief get storage that is blocked by garbage
  size_t getBlocked(void) const;

  /// \brief reads up to Bytes bytes from Fd and fills them into the Buffer
  ///
  /// This is non blocking.
  ///
  /// on error throws exception, otherwise returns amount of bytes filled
  ///
  /// \return amounts of bytes filled into buffer
  ssize_t fill(const int Fd, const size_t Bytes);

  /// \brief reads Bytes bytes from Buffer and sends them to Fd
  ///
  /// This is non blocking.
  /// When SIGPIPE would be sent instead an exception is thrown.
  ///
  /// on error throws exception, otherwise returns amount of bytes emptied into
  /// Fd
  ///
  /// \return amounts of bytes emptied into Fd
  ssize_t empty(const int Fd, const size_t Bytes);

  /// \brief optimizes Buffer for an attempt to transfer Bytes bytes through it
  ///
  /// So far this is an extremely simple algorithm
  void optimize(void);

private:
  char _buffer[BUFFER_SIZE]
  size_t _start;
  size_t _end;
};
