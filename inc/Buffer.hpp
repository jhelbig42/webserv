#pragma once

#include "CompileTimeConstants.hpp"
#include <string>
#include <sys/types.h>

/// \class Buffer
/// \brief This is a simple character buffer that can be filled from
/// and emptied into file descriptors.
///
/// Should mostly be usable without much thinking
///
///
/// Make sure to not confuse what the following funcionts do
/// getOccupied()
/// getUsed()
/// getFree()
/// getBlocked()
/// If you have better suggestions for names, let me know.
///
///
/// Good to know:
///
/// You can make a string from a buffer's content by calling
/// std::string s(buf.begin(), buf.end());
///
/// You can delete the first n bytes from a buffer by calling
/// buf.deleteFront(n);
///
///
/// Caveat:
///
/// When empty() or fill() returns 0 it is unknown if this is because the
/// buffer was empty/full or because the fd did not take input or give output.
/// You can call getFree() or getUsed() to determine the reason.
///
///
/// Improving performance:
///
/// The main performance consideration is to when to format the buffer
/// i.e. whenn to move all of it's data to the beginning of it.
///
/// fill() by itself only formats in the most naive cases.
///
/// 1) Usecase: for fixed n: fill(n) -> empty(n) loop
/// Use optimize() before each new call to fill().
/// optimize() is designed for this usecase and will format whenever
/// its algorithms deems it appropriate. You can look at it's implementation
/// to figure out the ideas or improve on them.
///
/// 2) Usecase: fill() -> remove data from buffer loop
/// Use format() when sensible i.e. when little data is in the buffer
/// and/or the data is close to the end of the buffer and/or there is a lot
/// to gain from formatting.
/// You can use getUsed(), getFree() and getBlocked() to figure these things out
/// or just use some heuristic.
class Buffer {
public:
  Buffer(void);
  Buffer(const Buffer &other);
  Buffer &operator=(const Buffer &other);
  ~Buffer(void);

  typedef size_t size_type;
  typedef char *iterator;
  typedef const char *const_iterator;

  char &operator[](size_type i);
  const char &operator[](size_type i) const;

  iterator begin(void);
  const_iterator begin(void) const;

  iterator end(void);
  const_iterator end(void) const;

  const static size_type size = BUFFER_SIZE;

  /// \brief get currently occupied storage in Buffer
  size_type getOccupied(void) const;

  /// \brief gets currently meaningfully occupied storage in Buffer
  size_type getUsed(void) const;

  /// \brief gets currently free storage in Buffer
  size_type getFree(void) const;

  /// \brief get storage that is blocked by garbage
  size_type getBlocked(void) const;

  /// \brief reads up to Bytes bytes from Fd and fills them into the Buffer
  ///
  /// This is non blocking.
  ///
  /// on error throws exception, otherwise returns amount of bytes filled
  ///
  /// \return amounts of bytes filled into buffer
  size_type fill(const int Fd, const size_t Bytes);

  /// \brief reads Bytes bytes from Buffer and sends them to Fd
  ///
  /// This is non blocking.
  /// When SIGPIPE would be sent instead an exception is thrown.
  ///
  /// on error throws exception, otherwise returns amount of bytes emptied into
  /// Fd
  ///
  /// \return amounts of bytes emptied into Fd
  size_type empty(const int Fd, const size_t Bytes);

  /// \brief moves meaningful data to the front of the buffer.
  void format(void);

  /// \brief formats buffer if it is deemed wise for performance gains
  ///
  /// So far this is an extremely simple algorithm
  void optimize(const size_t Bytes);

  /// \brief resets the buffer
  void reset(void);

  /// \deletes the first Bytes meaningful bytes from the buffer
  void deleteFront(const size_t Bytes);

private:
  char _buffer[BUFFER_SIZE];
  size_type _start;
  size_type _end;
};
