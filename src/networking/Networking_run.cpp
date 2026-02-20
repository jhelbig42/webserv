// TODO add 42 header

#include "Networking.hpp"
#include "NetworkingDefines.hpp"

// TODO These functions could be made static:
// process(), accept_connection(), add_connection_to_map()
// But then they would lose sight of networking::client_addr struct
// and netoworking::read_data().
// Decide on most elegant arrangement.

void networking::poll_loop(const int sock);
void networking::process(const int listen_sock,
		std::map<int, Connection> &c_map,
		std::vector<pollfd> &fds);
int networking::accept_connection(const int listen_sock, client_addr *candidate);
void networking::add_connection_to_map(const struct client_addr &candidate,
                                       std::map<int, Connection> &c_map);

// poll_loop() introduces the while(1) networking loop that will run
// for the duration of the webserver. This function:
//
// • Declares c_map, a map of clients (key = fd, value = Connection object)
// • Declares fds, a vector of pollfd structs
//  - Pollfd structs are library-defined and required by poll()
//  - Placing pollfds in a vector is useful for constant re-sizing,
//    as well as keeping constant track of the collection size.
//    Can ALSO be used as a normal array, which poll() expects.
// • Adds the server's listening socket as the first member of pollfd vector
// • Enters the polling loop, which
// 	- continuously checks and marks which pollfds are ready for I/O
// 	- processes the ones that are marked

void networking::poll_loop(const int sock) {

  std::map<int, Connection> c_map;
  std::vector<pollfd> fds;

  pollfd listener = {sock, POLLIN, 0};
  fds.push_back(listener);

  while (1) {
    int res = poll(fds.data(), fds.size(), -1); // -1 means poll indefinitely
    if (res == -1) {
    	std::ostringstream msg;
    	msg << "poll: " << std::strerror(errno);
      logging::log(logging::Error, msg.str());
      
      // TODO How to best handle poll() failure?
      // Currently, logging error and exiting
      // Could also log Warning and continue.
    }
    process(sock, c_map, fds);
  }
}



// process() iterates through the vector of fds and handles the flags
// that were set by poll() in fd[i]->events & fd[i]->revents.
//
// If the listening socket got a new connection, we attempt to accept the connection.
// If accept is successful, the connection is added to the map of connections
// and its fd is added to a stash. This stash will be appended to the fds vector
// after all current fds have been handled. 
//
// TODO More robust and granular error-handling, see additional flags in man pages
// Currently, the server logs Error and continues normally.
// We may want to downgrade some errors to warnings.
// We may want to drop connections with certain errors / flags? TBD.

void networking::process(const int listen_sock, std::map<int, Connection> &c_map,
                         std::vector<pollfd> &fds) {

  std::vector<pollfd> new_fd_batch;
  std::vector<int> delete_list;
  for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
	if (it->revents & POLLNVAL || it->revents & POLLERR) { // error
    	std::ostringstream msg;
    	msg << "process: " << "poll() resulted in unexpected results for fd " << it->fd
		<< " : POLLNVAL or POLLERR";
      logging::log(logging::Error, msg.str());
    }

    if (it->revents & (POLLIN | POLLHUP)) { // data to read | hang-up
      
	  std::cout << "POLLIN | POLLHUP : fd : " << it->fd << "\n";
	  if (it->fd == listen_sock) { // listening socket got new connection
	    std::cout << "is listener\n";
        client_addr candidate;
        if (accept_connection(listen_sock, &candidate) != -1) {
          add_connection_to_map(candidate, c_map);
          pollfd new_fd = {candidate.client_sock, POLLIN, 0};
          new_fd_batch.push_back(new_fd);
        }
      }
	
	else {

	    std::cout << "is client\n";
        std::map<int, Connection>::iterator c_it = c_map.find(it->fd);
        if (c_it != c_map.end()) {
          (c_it->second).read_data();
		} else {
          logging::log(logging::Error, "process: Connection not found in map "
                                       "container (This should never happen)");
	  				// could be removed after thorough testing
        }
      }
    }
/*
	if (it->revents & POLLNVAL || it->revents & POLLERR) { // error
    	std::ostringstream msg;
    	msg << "process: " << "poll() resulted in unexpected results for fd " << it->fd
		<< " : POLLNVAL or POLLERR";
      logging::log(logging::Error, msg.str());
    }

    if (it->revents & (POLLIN | POLLHUP)) { // data to read | hang-up
      
	  std::cout << "POLLIN | POLLHUP : fd : " << it->fd << "\n";
	  if (it->fd == listen_sock) { // listening socket got new connection
	    std::cout << "is listener\n";
        client_addr candidate;
        if (accept_connection(listen_sock, &candidate) != -1) {
          add_connection_to_map(candidate, c_map);
          pollfd new_fd = {candidate.client_sock, POLLIN, 0};
          new_fd_batch.push_back(new_fd);
        }
      }
	
	else {

	    std::cout << "is client\n";
        std::map<int, Connection>::iterator c_it = c_map.find(it->fd);
        if (c_it != c_map.end()) {
          (c_it->second).read_data();
		} else {
          logging::log(logging::Error, "process: Connection not found in map "
                                       "container (This should never happen)");
	  				// could be removed after thorough testing
        }
      }
    }
	*/
  }
  /*
  for (std::vector<int>::iterator it = delete_list.begin();
  	it != delete_list.end(); it++) {
  	fds.erase(it);
  }
  */
  fds.insert(fds.end(), new_fd_batch.begin(), new_fd_batch.end());
}


// accept_connections() is a a wrapper for accept(), which extracts
// the first connection request on the queue of pending connections
// for the listening socket. Creates new socket for the 
// incoming connection.
//
// RETURNS: fd for new socket

int networking::accept_connection(const int listen_sock, client_addr *candidate) {

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

void networking::add_connection_to_map(const struct client_addr &candidate,
                                       std::map<int, Connection> &c_map) {

  Connection new_connection =
      Connection(candidate.client_sock, candidate.addr, candidate.addr_size);
  c_map.insert(std::make_pair(candidate.client_sock, new_connection));
}
