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

#define MAX_EVENTS 4096

std::string get_time() {
    char buffer[80];
    std::time_t current_time = std::time(nullptr);
    struct tm* time_info = std::localtime(&current_time);
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", time_info);
    return (buffer);
}

#include <fstream>
std::string handle_get_request(const std::string& resource_path) {

    std::ifstream file(( "./data/www/" + resource_path).c_str());

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

std::string handle_post_request(const std::string& resource_path, std::string file_content, size_t file_content_size) {
    try {
        // Process the POST request based on the resource_path
        if (resource_path == "/upload") {
            // Handle the /upload_image endpoint
            // Save the uploaded image content to a file
            std::ofstream file("./data/www/uploaded_image.png", std::ios::binary);
            file.write(&file_content[0], file_content_size);
            std::cout << "FILE CONTENT: " << std::endl;
            std::cout.write(&file_content[0], file_content_size);
            file.close();
            // Return a success response
            return "Image uploaded successfully!";
        } else {
            // Handle other POST requests to unknown endpoints
            std::cerr << "Not found" << std::endl; // Debug
            throw std::runtime_error("Not found");
        }
    } catch (const std::exception& e) {
        // Handle any errors
        std::cerr << "Internal error" << std::endl; // Debug
        throw std::runtime_error("Internal error: " + std::string(e.what()));
    }
    // try {
    //     // Process the POST request based on the resource_path
    //     if (resource_path == "/submit_order") {
    //         // Handle the /submit_order endpoint
    //         // Extract parameters from the request body and process the order
    //         std::ifstream file("./data/www/order_success.html");
    //         std::stringstream file_contents;
    //         file_contents << file.rdbuf();
    //         //read the actual data
    //          HTTPResponse    h(200);
    //          h.setBody(file_contents.str());
    //         return h.getRawResponse();
    //     } else {
    //         // Handle other POST requests to unknown endpoints
    //         std::cerr << "Not found" << std::endl; //debug
    //         int             err = 404;
    //         HTTPResponse    h(err);
	//         h.setBody(generate_error_page(err));
    //         return (h.getRawResponse());
    //     }
    // } catch (const std::exception& e) {
    //     // Handle any errors
    //     std::cerr << "Internal error" << std::endl; //debug
    //         int             err = 500;
    //         HTTPResponse    h(err);
	//         h.setBody(generate_error_page(err));
    //         return (h.getRawResponse() + std::string(e.what()));
    // }
}

#include <iostream>
#include <sstream>
#include <string>
std::string process_request(char* request, size_t bytes_received) {
    HTTPRequestParser p(request, bytes_received);
    HTTPRequest req = p.parse();
    std::cerr << "REQUEST" << std::endl;
        std::cerr.write(&request[0], bytes_received);
       std::cerr << "REQUEST" << std::endl << std::endl << std::endl;
    // std::cerr << method << " " << uri << std::endl << "BOOODY: " << std::endl << body << std::endl  << std::endl; //debug
    if (req.method == "GET") {
        std::cerr << "GET REQUEST" << std::endl; //debug
        return (handle_get_request(req.path));
    }
    else if (req.method == "POST") {
        std::cerr << "POST REQUEST" << std::endl; //debug
        return (handle_post_request(req.path, req.body, std::stoul(req.headers.find("Content-Length")->second)));
    }
    else if (req.method == "DELETE") {
        std::cerr << "DELETE REQUEST" << std::endl; //debug
    }
    return "";
}

void handle_data(int client_fd) {
    char buffer[1024];
    ssize_t bytes_received;

    // Receive data from client
    bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        // Error or connection closed
        if (bytes_received == 0) {
            // Connection closed by client
            printf("Client disconnected.\n");
        } else {
            perror("recv");
        }
        close(client_fd);
    } else {
        // Process received data
        //std::string cpp_string(buffer);
       std::cerr << "BUFFER" << std::endl;
        std::cerr.write(buffer, bytes_received);
       std::cerr << "BUFFER" << std::endl << std::endl << std::endl;
        std::string processed_req = process_request(buffer, bytes_received);

        // Echo back to the client
        send(client_fd, processed_req.c_str(), processed_req.length(), 0);
    }
}

int create_and_bind_socket(const char *port) {
    struct sockaddr_in server_addr;
    int listen_fd;

    // Create a TCP socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Enable address reuse to avoid "Address already in use" error
    int enable = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(port));

    // Bind the socket to the specified port
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return listen_fd;
}

int main() {
    int epoll_fd = epoll_create(42); //could be anything but zero (it's ignored)
    if (epoll_fd == -1){;} //epoll error
    struct epoll_event event; /*The  epoll_event  structure specifies data
                                that the kernel should save and return when
                                the corresponding file descriptor becomes ready.
                                 
                                 struct epoll_event {
                                    uint32_t      events;   Epoll events 
                                    epoll_data_t  data;     User data variable
                                };*/
    struct epoll_event events[MAX_EVENTS];

    struct sockaddr_storage client_addr; /*sockaddr structure that is not
                                        associated with address family,
                                        instead it is large enough to be   
                                        keep info for a sockaddr of any type
                                        within a larger struct
                                        
                                        Socket Structure        Address Family
                                        struct sockaddr_dl      AF_LINK
                                        struct sockaddr_in      AF_INET
                                        struct sockaddr_in6     AF_INET6
                                        struct sockaddr_ll      AF_PACKET
                                        struct sockaddr_un      AF_UNIX*/

    socklen_t client_addrlen = sizeof(client_addr);

    // Set up listening socket and add to epoll
    int listen_fd = create_and_bind_socket("8080"); //created function (look up ^)
    listen(listen_fd, SOMAXCONN); /*Marks the socket as a passive
                                socket (it would only accept incoming
                                    connection requests)*/
    event.events = EPOLLIN;
    /*EPOLLIN
       The associated file is available for read(2) operations.*/
    event.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event); /*This system call is
                                    used to add, modify, or remove entries
            EPOLL_CTL_ADD
              Add an entry to the interest list of the epoll file
              descriptor, epfd.  The entry includes the file descriptor,
              fd, a reference to the corresponding open file description
              (see epoll(7) and open(2)), and the settings specified in
              event.*/

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        /*The epoll_wait() system call waits for events on the epoll(7)
       instance referred to by the file descriptor epfd.  The buffer
       pointed to by events is used to return information from the ready
       list about file descriptors in the interest list that have some
       events available.  Up to maxevents are returned by epoll_wait().
       The maxevents argument must be greater than zero.*/
       
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
    return 0;
}
