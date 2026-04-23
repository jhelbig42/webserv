### The Task
Developing a fully functional HTTP/1.0 server from scratch, capable of handling multiple non-blocking client connections.

Implementing I/O Multiplexing (using poll(), select(), or kqueue()) to manage concurrent requests efficiently without thread overhead.

Designing a robust HTTP Request/Reaction parser and a configuration file system, emphasizing OOP principles and clean class architecture.

Handling CGI execution, file uploads, and various HTTP methods (GET, POST, DELETE).

### Config Parsing

#### comments
- every line where the first non whitespace charater is `#` is considered to be a comment and ignored during parsing

#### general structure

On the top level there are server objects
```
server {
    # entries
}
```

##### server entries
each server object can have multiple entries. The following entries are available:
- `listen`
- `root`
- `autoindex`
- `allow`
- `error_pages`
- `max_request_body`
- `location`

###### general
each entry (except `location`) needs to be followd by whitespace and then one or several "arguments" followed by a semicolon
```
server{
    listen 192.168.56.3:8080;
    max_request_body    1024   ;
    error_pages
        404 500 /generic_error.html
        ;}
```
the different spacing in the example demonstrated that whitespace in many places is irrelevant

###### location

`location` entries follow a syntax similar to the one of `server` and there can be nested locations:
```
server {
    # server entries
    location /location/path/ {
        # location entries
    }
    location /scripts/ {
        # location
        location /*.php {
            # location entries
        }
        location /*.sh {
            # location entries
        }
        location /*.py {
            # location entries
        }
    }
}
```
all server entries except `listen` can also be used as location entries. But locations can have three additional entries which are invalid at the server level. These are:
- `cgi`
- `redirect`
- `return`

When a resource is requested it is matched against the paths of all locations of the current `server` or `location` object. If there is a match the location object is "entered". Then the settings of this location object are considered possibly descending into a even deeper nested location object.

In most cases the entries of deeper nested objects are overwriting entries of their parents. An exception are configured error pages. Those are additive. A parent object's error page only gets overwritten when there is a conflict for the same error code.

#### entries

If there are duplicate entries the later ones or deeper nested ones are usually overwriting earlier or not as deeply nested ones.

##### server specific entries

###### listen
Defines interface port pairs on which the server should be listening. If added multiple times this is additive and the server will attempt to listen at multiple interface port pairs.

*example:*
```
interface 192.168.56.3:8080;
```

##### location specific entries
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

##### general entries

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

###### default (NOT YET IMPLEMENTED)
Default resource to be served.
*example:*
```
default index.html;
```
