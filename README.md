### The Task
Developing a fully functional HTTP/1.0 server from scratch, capable of handling multiple non-blocking client connections.

Implementing I/O Multiplexing (using poll(), select(), or kqueue()) to manage concurrent requests efficiently without thread overhead.

Designing a robust HTTP Request/Reaction parser and a configuration file system, emphasizing OOP principles and clean class architecture.

Handling CGI execution, file uploads, and various HTTP methods (GET, POST, DELETE).

### Config Parsing

- when no allowd methods are specified every method is allowed
- allowed methods in locations overwrite those of parent entities (servers or locations)
- when a location is selected matches are first ordered by length of the location path (descending) and then lexicographically by ascii values (ascending)
