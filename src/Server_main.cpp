#include "../include/HandleData.hpp"

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
    int enable = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

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

void    ServerLoop(Http httpConf) {
    int epoll_fd = epoll_create(42);
    std::vector<int>    listen_fds;
    if (epoll_fd == -1){;} //epoll error
    struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    std::vector<Server>::const_iterator it;
    for (it = httpConf.servers.begin(); it != httpConf.servers.end(); ++it) {
        std::cerr << "PORT: " <<  it->GetPort().c_str() << std::endl;
        int listen_fd = create_and_bind_socket(it->GetPort().c_str());
        listen(listen_fd, SOMAXCONN);
        event.events = EPOLLIN;
        event.data.fd = listen_fd;
        event.data.ptr = &it;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);
        listen_fds.push_back(listen_fd);
    }

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_SEC * 1000);
        if (num_events == 0) {
            //timeout (handle other tasks, prevent dead-locks)
            std::cerr << "Timeout" << std::endl;
        }
        
        for (int i = 0; i < num_events; i++) {
            // Check if the current event's file descriptor is in listen_fds
            bool found_fd = false;
            for (size_t j = 0; j < listen_fds.size(); ++j) {
                if (events[i].data.fd == listen_fds[j]) {
                    found_fd = true;
                    // Accept incoming connection
                    int client_fd = accept(listen_fds[j], (struct sockaddr *)&client_addr, &client_addrlen);
                    if (client_fd == -1) {
                        // Handle error
                        // (You might want to log the error or handle it appropriately)
                        continue;
                    }
                    // Add new client socket to epoll
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                    break;  // Exit the loop once the file descriptor is found
                }
            } if (!found_fd) {
                handle_data(events[i].data);
            }
        }
    } close(epoll_fd);
}


int main() {
    
    std::string nginxConfig = "./data/webserv.default.conf";
    ServerConfig sc(nginxConfig);
    ServerLoop(sc.GetHttps()[0]);
    return 0;
}
