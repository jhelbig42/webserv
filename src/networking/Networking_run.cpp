// TODO add 42 header

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

void networking::poll_loop(const int sock);

// poll_loop() introduces the while(1) networking loop that will run
// for the duration of the webserver. This function:
//
// • Creates a map of clients (key = fd, value = Connection object)
// • Creates a vector of fd structs
//  - These structs are library-defined and required by poll()
//  - Placing them in a vector is useful for constant re-sizing of fd collection,
//    as well as keeping constant track of the collection size.
//
// polls all existing connections via their corrsponding sockets,
// interchangeably referred to as fds, to check if any one is ready for I/O
//

void networking::poll_loop(const int sock) {

  static int fd_count;             // counter for # of active clients
  std::map<int, Connection> c_map; // empty map of Connection objects
  std::vector<pollfd> fds; // empty vector of poll_fd structs (see <poll.h>)

  pollfd listener = {sock, POLLIN, 0}; // create pollfd for listening socket
  fds.push_back(listener);             // add to vector
  fd_count++;

  (void)c_map;

  while (1) {
    int res = poll(fds.data(), fds.size(), -1); // poll indefinitely (allowed?)
    if (res == -1) {
      // logging::log(Error, "poll: ", std:sterror(errno)); // TODO after merge
      logging::log(logging::Error, "poll: ");
      exit(1); // Should exit or continue?
    }
    process(sock, c_map, fds); // process results of poll
  }
}

// This function follows Beej closely
void networking::process(int listen_sock, std::map<int, Connection> &c_map,
                         std::vector<pollfd> &fds) {

  std::vector<pollfd> new_fd_batch;
  for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
    if (it->revents & POLLNVAL || it->revents & POLLERR) {
      logging::log(logging::Error,
                   "polling file descriptor gave unexpected results");
      // TODO print fd after merge
      // TODO more robust and granular error-handling, see additional flags in
      // the man pages & double-check subject, re: errno reactions
      exit(1);
    }
    if (it->revents & (POLLIN | POLLHUP)) {
      // is there something to read, or did someone hang up on us?
      if (it->fd == listen_sock) { // listening socket got new connection

        client_addr candidate;
        if (accept_connection(listen_sock, &candidate) != -1) {
          add_connection_to_map(candidate, c_map);
          pollfd new_fd = {listen_sock, POLLIN, 0};
          new_fd_batch.push_back(new_fd);
        }
      } else {
        std::map<int, Connection>::iterator c_it = c_map.find(it->fd);
        if (c_it != c_map.end()) {
          read_data(listen_sock, it->fd, c_it->second, fds);
        } else {
          logging::log(logging::Error, "process: Connection not found in map "
                                       "container (This should never happen)");
        }
      }
    }
  }
  fds.insert(fds.end(), new_fd_batch.begin(), new_fd_batch.end());
}

int networking::accept_connection(int listen_sock, client_addr *candidate) {

  candidate->client_sock = accept(
      listen_sock, (struct sockaddr *)&candidate->addr, &candidate->addr_size);
  if (candidate->client_sock == -1) {
    std::ostringstream msg;
    msg << "accept: " << std::strerror(errno)
        << " (can continue trying to accept connections)";
    logging::log(logging::Error,
                 msg.str()); // may downgrade log level at some point
    return (-1);
  }
  std::cout << "\nConnection accepted on socket " << candidate->client_sock
            << "\n\n"; // TODO move to debug
  return (0);
}

void networking::add_connection_to_map(struct client_addr &candidate,
                                       std::map<int, Connection> &c_map) {

  Connection new_connection =
      Connection(candidate.client_sock, candidate.addr, candidate.addr_size);
  c_map.insert(std::make_pair(candidate.client_sock, new_connection));
}
