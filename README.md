# Webserv Project

This project is about writing your own HTTP server in C++98. You will be able to test it with an actual browser. The purpose of this project is to gain a deeper understanding of how HTTP servers work and the protocols involved.

## Summary:

Project Title: Webserv
Description: An HTTP server written in C++98
Features:
    Parses configuration files
    Listens on multiple ports
    Handles GET, POST, and DELETE requests
    Serves static files
    Provides directory listings (configurable)
    Executes CGI scripts (basic)
    Uploads files (configurable)
    Handles errors with appropriate status codes
    Uses non-blocking I/O for efficient handling of multiple clients

### General Coding Guidelines:

    Compile your code with c++ and the flags -Wall -Wextra -Werror
    Your code must comply with the C++ 98 standard
    Use descriptive variable and function names
    Write well-commented code

## Installation

### Building the Project:

The project uses a Makefile to manage compilation and execution.

### Installation Steps:

    Clone the Repository:

    ```
    git clone git@github.com:VulpesDev/WebServer.git ~/bin/DEATH_serv && cd ~/bin/DEATH_serv && make
    ```
    <sub>Use code with caution.</sub>

This will clone the project's source code into a new directory named DEATH_serv (or the name you choose) in your ~/bin folder and then create an executable named ./webserv using Makefile.

## Configuration File Format And Examples

### Configuration File Format:

This document describes the format for the configuration file used by the Webserv project. The configuration file allows you to define various settings for your web server, including server behavior, listening ports, and how to handle different URLs.

```
Command lines must end in a semicolumn,
 commenting after or putting instructions in single line is considered parsing error

 Lines beginning with "#" would be ignored (the whole line there is no */ /*)

 Every entry level must be encapsulated by bracket pairs
  1.1 This applies to this 3 key symbols for encapsulation "[{()}]"

Any unrecognised input would be ignored except if it's the first word in line,
then it would throw an error as "unrecognised command"

Level encapsulation key words (they are encapsulated using '{}')
 -"http"
 -"server"
 -"location"

Key symbols
 -'{'
 -'}'
 -';'
 -'='
 -'#'

 Key word values (in server context)
  -"server_name [array of strings (space separated)]"
  -"client_max_body_size [int]" (ends wih either K/M/G for kilobyte/megabyte/giga)
  -"listen [int]"
  -"error_page [array of int]"

Key word values (in location context)
  -"limit_except [space separated methods to limit]"
  -"return [status] [text(optional)]"
  -"root [text]"
  -"autoindex [0 or 1]"
  -"index [text]"
  -"fastcgi_pass [text]"

```

### Default Configuration File:

The configuration file(./config/webserv.default.conf) defines various server settings, listening ports, and how to handle URLs. Here's the default configuration file with explanations:
It defines a single Webserver instance listening on port 8080 (server_name "load-balanc").

```
http {
    server {
        listen 8080;
        server_name load-balanc;

		client_max_body_size 2M;

        error_page 404 ./data/error_pages/not_found.html;
        error_page 500 ./data/error_pages/hey.html;
        error_page 403 ./data/error_pages/forbidden.html;

        location / {
            index index.html;
            autoindex 1;
            fastcgi_pass data/www/cgi-bin/;
        }

        location /index.html {
            limit_except GET;
            fastcgi_pass data/www/cgi-bin/;
        }
        location /upload {
            limit_except POST;
        }
        location /cgi-bin/ {
            index index.html;
            fastcgi_pass data/www/cgi-bin/;
        }
        location /basic.php {
            fastcgi_pass ./data/www/cgi-bin/;

        }
        location /test_redirect {
            return 301 /cgi-bin/basic.php;
        }
    }
}
```

Error Handling: Sets custom error pages for 404 (Not Found), 500 (Internal Server Error), and 403 (Forbidden) errors.
Default Location (/): Serves index.html with directory listings enabled and passes requests to FastCGI programs (data/www/cgi-bin/).
Specific Locations:
    /index.html: Limits access to GET requests and also passes to FastCGI (might be redundant).
    /upload: Likely handles file uploads, allowing only POST requests.
    /cgi-bin/: Serves index.html for directories and passes requests to FastCGI programs.
    /basic.php: Directly passes requests to a FastCGI program (likely a script).
    /test_redirect: Permanently redirects to /cgi-bin/basic.php.

### Two Web Servers on Port 8080:

This configuration defines two separate server instances within the http section, both listening on port 8080:
```
http {
    server {
        listen 8080;
        server_name load-balanc;

		client_max_body_size 2M;
        location / {
            return 308 index.html;
        }
    }
    server {
        listen 8080;
        server_name def_index;

		client_max_body_size 1K;

        location / {
            return 308 test.html;
            
        }
    }
}
```

Server 1 (server_name "load-balanc")
    This server responds to requests where the Host header in the HTTP request matches "load-balanc".
    client_max_body_size 2M: Sets the maximum allowed size (2 Megabytes) for request bodies sent to this server.
    location /: This location block applies to all requests.
        return 308 index.html: Any request to this server (regardless of the specific path) results in a permanent redirect (status code 308) to the file index.html.
Server 2 (server_name "def_index")
    This server responds to requests where the Host header in the HTTP request matches "def_index".
    client_max_body_size 1K: Sets a smaller maximum body size (1 Kilobyte) for this server.
    location /: This location block also applies to all requests.
        return 308 test.html: Any request to this server triggers a permanent redirect (status code 308) to the file test.html.
How it Works:

A client makes an HTTP request to port 8080.
The Webserver examines the Host header in the request:
    If the header matches "load-balanc", Server 1's configuration is applied (redirect to index.html).
    If the header matches "def_index", Server 2's configuration is applied (redirect to test.html).
    If the Host header is missing or doesn't match either server name, the behavior might be undefined depending on the project's implementation.

## Testing
```
curl http://localhost:8080/  # Should redirect to index.html (status code 308)
curl -I http://localhost:8080/  # Use the -I flag to see only the header response, including the redirect status code

```
```
curl GET http://localhost:8080/  # Simulate a GET request
curl DELETE http://localhost:8080/delete "This is some data"  # Delete data
```
<sub>To test the same port config</sub>
```
curl -H "Host: def_index" http://localhost:8080/  # Set the Host header to match Server 2
curl -I -H "Host: def_index" http://localhost:8080/  # View headers with redirect for Server 2

```
