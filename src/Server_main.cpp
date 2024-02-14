#include <HttpMessage.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctime>
#include <ServerConfig.hpp>

#define MAX_EVENTS 4096
#define TIMEOUT_SEC 10 //5-10 seconds is apparently common according to chatGPT 

#include <fstream>
/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message
std::string handle_get_request(const std::string& resource_path) {

    std::ifstream file(( "./data/www" + resource_path).c_str());

    if (file.is_open()) {
        std::cerr << "Opening file" << std::endl; //debug
        // Read the contents of the file
        std::stringstream file_contents;
        file_contents << file.rdbuf();
        std::cerr << "Creating a response" << std::endl; //debug

        // Build the HTTP response
        HTTPResponse    h(200);
        h.setBody(file_contents.str());
        file.close();

        return (h.getRawResponse());
    } else {
        // Resource not found
        std::cerr << "Not found" << std::endl; //debug
        int             err = 404;
        HTTPResponse    h(err);

	    h.setBody(generate_error_page(err));

        return (h.getRawResponse());
    }

    return "";
}

/// @brief gets the boundary of the multipart form data
/// @param content_type the line of the header part of the request
/// @return the boundary as a std::string
std::string get_boundary(const std::string& content_type) {
    std::string boundary_tag = "boundary=";
    size_t pos = content_type.find(boundary_tag);
    if (pos != std::string::npos) {
        return content_type.substr(pos + boundary_tag.length());
    }
    throw std::runtime_error("Boundary not found in Content-Type header");
}

/// @brief extracts the data from png content by searching the start/end boundary
/// @param file_content raw body content
/// @param boundary the boundary returned by get_boundary()
/// @return raw data pf the png
std::string extract_png_data(const std::string& file_content, const std::string& boundary) {
    std::string png_data;
    std::string boundary_str = "--" + boundary;
    std::string boundary_end_str = "--" + boundary + "--";

    size_t start_pos = file_content.find(boundary_str);
    if (start_pos == std::string::npos) {
        throw std::runtime_error("Start boundary not found");
    }
    start_pos = file_content.find("\r\n\r\n", start_pos);
    if (start_pos == std::string::npos) {
        throw std::runtime_error("End of headers not found");
    }
    start_pos += 4;

    size_t end_pos = file_content.find(boundary_end_str, start_pos);
    if (end_pos == std::string::npos) {
        throw std::runtime_error("End boundary not found");
    }

    png_data = file_content.substr(start_pos, end_pos - start_pos);

    return png_data;
}

/// @brief handles the post request
/// @param resource_path the uri in the request
/// @param file_content the body of the request 
/// @return the raw response message
std::string handle_post_request(const std::string& resource_path, const std::string& file_content) {
    try {
        // Process the POST request based on the resource_path
        if (resource_path == "/upload") {
            // Parse the Content-Type header to get the boundary
            std::string content_type = "Content-Type: multipart/form-data; boundary=";
            std::string boundary = get_boundary(content_type);

            // Extract PNG data from the multipart form-data
            std::string png_data = extract_png_data(file_content, boundary);

            // Save the PNG data to a file
            std::ofstream file("./data/www/uploaded_image.png", std::ios::binary);
            file.write(png_data.c_str(), png_data.size());
            file.close();

            // Return a success response
            HTTPResponse    h(200);
            return h.getRawResponse();
        } else {
            // Handle other POST requests to unknown endpoints
            int             err = 404;
            HTTPResponse    h(err);
	        h.setBody(generate_error_page(err));
            return h.getRawResponse();
        }
    } catch (const std::exception& e) {
        // Handle any errors
        std::cerr << "Internal error: " << e.what() << std::endl; // Debug
        int             err = 500;
        HTTPResponse    h(err);
	    h.setBody(generate_error_page(err));
        return h.getRawResponse();
    }
}

/// @brief handles the delete request
/// @param resource_path the uri passed in the request
/// @return the raw response message
std::string handle_delete_request(const std::string& resource_path) {
    try {
        // Process the DELETE request based on the resource_path
        if (resource_path == "/delete") {
            const char* file_to_delete = "./data/www/uploaded_image.png";

            int result = std::remove(file_to_delete);
            if (result == 0) {
                // File deleted successfully
                HTTPResponse    h(200);
                return h.getRawResponse();
            } else {
                // Error deleting file
                int             err = 503;
                HTTPResponse    h(err);
	            h.setBody(generate_error_page(err));
                return h.getRawResponse();
            }
        } else {
            int             err = 404;
            HTTPResponse    h(err);
	        h.setBody(generate_error_page(err));
            return h.getRawResponse();
        }
    } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << std::endl; // Debug
        int             err = 500;
        HTTPResponse    h(err);
	    h.setBody(generate_error_page(err));
        return h.getRawResponse();
    }
}

#include <iostream>
#include <sstream>
#include <string>
/// @brief checks the method being passed and calls the functions accordingly
/// @param request the raw request
/// @param bytes_received number of bytes received
/// @return the raw response message
std::string process_request(char* request, size_t bytes_received) {
    HTTPRequestParser p(request, bytes_received);
    HTTPRequest req = p.parse();
    if (req.method == "GET") {
        std::cerr << "GET REQUEST" << std::endl; //debug
        return (handle_get_request(req.path));
    }
    else if (req.method == "POST") {
        std::cerr << "POST REQUEST" << std::endl; //debug
        return (handle_post_request(req.path, req.body));
    }
    else if (req.method == "DELETE") {
        std::cerr << "DELETE REQUEST" << std::endl; //debug
        return (handle_delete_request(req.path));
    }
    return "";
}

std::pair<std::string, ssize_t> receive_all(int client_fd) {
    std::string received_data;
    char buffer[MAX_CHUNK_SIZE];
    ssize_t bytes_received;
    ssize_t total_bytes_received = 0;

    do {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return std::make_pair("", -1); // Error occurred
        }
        if (bytes_received == 0) {
            // Connection closed by client
            break;
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);
    } while (bytes_received == MAX_CHUNK_SIZE);

    return std::make_pair(received_data, total_bytes_received);
}

void handle_data(int client_fd) {
    std::pair<std::string, ssize_t> received_info = receive_all(client_fd);
    std::string received_data = received_info.first;
    ssize_t total_bytes_received = received_info.second;

    // Process the received data
    // std::cerr << "Total bytes received: " << total_bytes_received << std::endl;
    // std::cerr << "Received data: "<< std::endl;
    // std::cerr.write(&received_data[0], total_bytes_received);
    // std::cerr << std::endl;
    // std::cerr << "END Received data ----------------"<< std::endl;
    
    std::string processed_req;
    try {
        processed_req = process_request(&received_data[0], total_bytes_received);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    send(client_fd, processed_req.c_str(), processed_req.length(), 0);
    close(client_fd);
}

#include <fcntl.h>
int create_and_bind_socket(const char *port) {
    struct sockaddr_in server_addr;
    int listen_fd;

    // Create a TCP socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // ! ! ! THIS IS NOT ALLOWED BY THE SUBJECT
    // // Enable address reuse to avoid "Address already in use" error
    // int enable = 1;
    // if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }

    int flags = fcntl(listen_fd, F_GETFL, 0); //Get the flags that already exist
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    if (fcntl(listen_fd, F_SETFL, F_GETFL | O_NONBLOCK) == -1) { //Add on to them (non-blocking)
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //indicating IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //socket will bind to any available network interface
    server_addr.sin_port = htons(atoi(port)); //setting the port
    // Bind the socket to the specified port
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    return listen_fd;
}

void    ServerLoop(Server serv) {
    int epoll_fd = epoll_create(42); //42 is just ignored
    if (epoll_fd == -1){;} //epoll error
    struct epoll_event event; //the listen event
    struct epoll_event events[MAX_EVENTS]; // all the other clients

    struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    // Set up listening socket and add to epoll
    std::cerr << "PORT: " <<  serv.GetPort().c_str() << std::endl;
    int listen_fd = create_and_bind_socket(serv.GetPort().c_str());
    listen(listen_fd, SOMAXCONN); /*Marks the socket as a passive
                                socket (it would only accept incoming
                                    connection requests)*/
    event.events = EPOLLIN;
    /*EPOLLIN
       The associated file is available for read(2) operations.*/
    event.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event); /*This system call is
                                    used to add, modify, or remove entries*/

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_SEC * 1000);
        /*The epoll_wait() system call waits for events on the epoll(7)
       instance referred to by the file descriptor epfd.  The buffer
       pointed to by events is used to return information from the ready
       list about file descriptors in the interest list that have some
       events available.  Up to maxevents are returned by epoll_wait().
       The maxevents argument must be greater than zero.*/
        if (num_events == 0) {
            //timeout (handle other tasks, prevent dead-locks)
            std::cerr << "Timeout" << std::endl;
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == listen_fd) {
                // Accept incoming connection
                int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                    &client_addrlen);
                // Add new client socket to epoll
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            } else {
                // Handle data on existing connection
                handle_data(events[i].data.fd);
            }
        }
    }

    close(epoll_fd);
}


int main() {
    
    std::string nginxConfig = "./data/webserv.default.conf";
    ServerConfig sc(nginxConfig);
    Server  serv = sc.GetFirstServer();
    ServerLoop(serv);
    return 0;
}
