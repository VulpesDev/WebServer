#include "HandleData.hpp"

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
    std::vector<Server>   listen_confs;
    if (epoll_fd == -1){;} //epoll error

    struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    struct epoll_event events[MAX_EVENTS];

    for (int i = 0; i < MAX_EVENTS; i++) {
        events[i].data.ptr = NULL;
    }

    std::vector<Server>::iterator it;
    for (it = httpConf.servers.begin(); it != httpConf.servers.end(); ++it) {
        struct epoll_event event;
        int listen_fd = create_and_bind_socket(it->GetPort().c_str() );
        listen(listen_fd, SOMAXCONN);
        event.events = EPOLLIN;
        it->setFd(listen_fd);
        event.data.ptr = new Server(*it);
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);
        listen_confs.push_back(*((Server *)event.data.ptr));
    }
    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT_SEC * 1000);
        if (num_events == 0) {
            //timeout (handle other tasks, prevent dead-locks)
            std::cerr << "Timeout" << std::endl;
            continue;
        }
        
        for (int i = 0; i < num_events; i++) {
            std::cerr << "event fd: " << ((Server*)events[i].data.ptr)->getFd() << std::endl;
            bool found_fd = false;
            for (size_t j = 0; j < listen_confs.size(); ++j) {
                if (((Server*)events[i].data.ptr)->getFd() == listen_confs[j].getFd()) {
                    found_fd = true;
                    int client_fd = accept(listen_confs[j].getFd(), (struct sockaddr *)&client_addr, &client_addrlen);
                    if (client_fd == -1) {
                        // Handle error
                        continue;
                    }
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    Server*  s = new Server(listen_confs[j]);
                    s->setFd(client_fd);
                    event.data.ptr = s;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                    break;
                }
            } if (!found_fd) {
                handle_data(events[i].data);
            }
        }
    } close(epoll_fd);
}


int main(int argc, char *argv[]) {
    std::string nginxConfig = "./config/webserv.default.conf";
    if (argc == 2) {
        nginxConfig = argv[1];
    }
    try {
        ServerConfig sc(nginxConfig);
        ServerLoop(sc.GetHttps()[0]);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    // std::cerr << "PRINTING VALUES" << std::endl;
    // sc.GetHttps()[0].servers[0].printValues();
    // std::cerr << "----|||||||||||----" << std::endl;
    return 0;
}