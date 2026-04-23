### The Task
Developing a fully functional HTTP/1.0 server from scratch, capable of handling multiple non-blocking client connections.

Implementing I/O Multiplexing (using poll(), select(), or kqueue()) to manage concurrent requests efficiently without thread overhead.

Designing a robust HTTP Request/Reaction parser and a configuration file system, emphasizing OOP principles and clean class architecture.

Handling CGI execution, file uploads, and various HTTP methods (GET, POST, DELETE).

### Config Parsing

#### comments
- every line where the first non whitespace charater is `#` is considered to be a comment and ignored during parsing

#### server
- when no allowed methods are specified no methods are allowed
- `autoindex` default to `on`
- `max_request_body` default to `1024`

#### Locations
- locations allow more finegrained configuration for different resources
- locations can be of four different types:
    - Redirect
    - Cgi
    - Return
    - None
- each location can only have one type
- the default type for locations is None
- only locations of type Redirect or None can nest other locations
- allowed methods in locations overwrite those of parent entities (servers or locations)
- when a location is selected matches are first ordered by length of the location path (descending) and then lexicographically by ascii values (ascending)
