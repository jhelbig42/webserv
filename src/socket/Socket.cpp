#include "Socket.hpp"
#include "Website.hpp"

Socket::Socket(const int Fd, const bool Listening, const Website &web) : _fd(Fd), _listening(Listening), website(web) {}