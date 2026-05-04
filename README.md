### The Task
Developing a fully functional HTTP/1.0 server from scratch, capable of handling multiple non-blocking client connections.

Implementing I/O Multiplexing (using poll(), select(), or kqueue()) to manage concurrent requests efficiently without thread overhead.

Designing a robust HTTP Request/Reaction parser and a configuration file system, emphasizing OOP principles and clean class architecture.

Handling CGI execution, file uploads, and various HTTP methods (GET, POST, DELETE).

### Config Parsing

#### comments
- every line where the first non whitespace charater is `#` is considered to be a comment and ignored during parsing

#### whitespace
- whitespace is used to separate directives (see below) from their arguments and the arguments from each other. All other whitespace is irrelevant to the parser.

#### general structure

There are two different contexts, `server` and `location`.
- `server` contexts need to start at the top level of the config file.
- `location` contexts can only be nested in `server` contexts or other `location` contexts
- `server` and `location` contexts can contain zero or more directives

In the following example the `listen`, `max_request_body`, `redirect` and `cgi` directives are used
```
server {
    listen 192.168.10.3:80;
    max_request_body 2048;
    location /scripts/ {
        max_request_body 1024;
    }
    location /images/ {
        redirect /001/;
    }
    location /*.php {
        cgi /usr/bin/php-cgi;
    }
}
```

Each server context specifies settings for a server entity that listens at specific interfaces. Directives and nested `location` contexts further specify the servers behavior.

When an HTTP request arrives at an interface specified through the `listen` directive in a server context, first the correct context gets determined.

##### determining the correct context for handling an HTTP request

- The context is selected depending on the resource field of an arriving HTTP request.
- The default context is the server that listens on the interface the HTTP request arrived on. After that the context gets determined by recursively descending into matching location contexts.
- A location context is considered matching when one of the two following things holds:
    1. The path given after the `location` keyword ends with `/` and is a prefix of the request's resource field.
    2. The path given after the `location` does not end with `/` and is a full match of the request's resource field. '*' in the location's path are considered wild cards in this case.

##### directives

Generally `speaking` `server` and location contexts can have the same directives. Only the `listen` directive is exclusive to `server` contexts.

In most cases the entries of deeper nested objects are overwriting entries of their parents. An exception are configured error pages. Those are additive. A parent object's error page only gets overwritten when there is a conflict for the same error code.

###### listen
Defines interface port pairs on which the server should be listening. If added multiple times this is additive and the server will attempt to listen at multiple interface port pairs.

*example:*
```
interface 192.168.56.3:8080;
```

###### cgi
Defines an executable that should be called as a cgi for resources in the given location.

*example:*
```
cgi /usr/bin/php-cgi;
```

###### redirect
Defines a path or resource that gets substituted for the part of the requested resource that matches the location path.

*example:*
```
location /asdf/ {
    redirect /qwer/;
}
location /*.php {
    redirect /script.py;
}
```
The resource `/asdf/hello` would be redirected to `/qwer/hello`.
The resource `/qwer/script.php` would be redirected to `/script.py`.

###### return
If part of a location this couses the server to directly send an HTTP response with a specified code and a specified resource for every resource in the given location.

*example:*
```
return 301 http://igotmoved.com;
```

###### root
Defines the system path relative to which resources are to be found. Defaults to `/` (which probably is a bad idea).

*example:*
```
root /var/www/;
```

###### autoindex
Defines if directory listing is enabled. Defaults to `on`.
*examples:*
```
server {
    autoindex on;
}
server {
    autoindex off;
}
```

###### allow
Defines allowed HTTP methods `*` allows all implemented methods (POST, GET, DELETE, HEAD).
*example:*
```
allow POST GET;
```

###### error_pages
Defines which pages will be served for a specific HTTP status code.
*example:*
```
error_pages 404 500 /default_error.html;
```

###### max_request_body
Defines the maximum accepted size in bytes of the body of HTTP requests sent to the server.
*example:*
```
max_request_body 512;
```

###### index
Default resource to be served.
*example:*
```
index index.html;
```
