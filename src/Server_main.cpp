#include <HandleData.hpp>

struct ClientConnection {
    int fd;
    Server* server;
    time_t last_activity_time;
};

int create_and_bind_socket(const char *port) {
    struct sockaddr_in server_addr;
    int listen_fd;

    // Create a TCP socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
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

int    new_connection(int epoll_fd, struct epoll_event& event, std::vector<Server> listen_confs, std::vector<ClientConnection>& active_clients) {
    struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    int found_fd = 0;

    for (size_t j = 0; j < listen_confs.size(); ++j) {
        if (((Server*)event.data.ptr)->getFd() == listen_confs[j].getFd()) {
            found_fd = 1;
            int client_fd = accept(listen_confs[j].getFd(), (struct sockaddr *)&client_addr, &client_addrlen);
            if (client_fd == -1)
                return -1;
            struct timeval tv;
            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = 0;
            if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) == -1){
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            struct epoll_event event;
            event.events = EPOLLIN | EPOLLET;
            Server*  s = new Server(listen_confs[j]);
            s->setFd(client_fd);
            event.data.ptr = s;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            ClientConnection connection = {client_fd, s, time(0)};
            active_clients.push_back(connection);
            break;
        }
    }
    return found_fd;
} 

void handle_hanging_requests(int epoll_fd, std::vector<ClientConnection>& active_clients) {
    time_t current_time = time(NULL);
    
    for (std::vector<ClientConnection>::iterator it = active_clients.begin(); it != active_clients.end(); ) {
        if (current_time - it->last_activity_time >= TIMEOUT_SEC) {
            close(it->fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->fd, NULL);
            delete(active_clients[it - active_clients.begin()].server);
            it = active_clients.erase(it);
        } else {
            ++it;
        }
    }
}

bool find_string_in_vector(const std::string& s, const std::vector<std::string>& v) {
    for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (*it == s)
            return true;
    }
    return false;
}


void    ServerLoop(Http httpConf) {
    int epoll_fd = epoll_create(42);
    if (epoll_fd == -1){;} //epoll error

    std::vector<std::string>   listen_ports;
    std::vector<Server>   listen_confs;
    std::vector<ClientConnection> active_clients;

    struct epoll_event events[MAX_EVENTS];

    for (int i = 0; i < MAX_EVENTS; i++) {
        events[i].data.ptr = NULL;
    }

    std::vector<Server>::iterator it;
    for (it = httpConf.servers.begin(); it != httpConf.servers.end(); ++it) {
        if (find_string_in_vector(it->GetPort(), listen_ports)) {
            continue;
        }
        struct epoll_event event;
        int listen_fd = create_and_bind_socket(it->GetPort().c_str() );
        listen(listen_fd, SOMAXCONN);
        listen_ports.push_back(it->GetPort());
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
            handle_hanging_requests(epoll_fd, active_clients);
            std::cerr << "Timeout" << std::endl;
            continue;
        }
        
        for (int i = 0; i < num_events; i++) {
             if (!new_connection(epoll_fd, events[i], listen_confs, active_clients)) {
                if(handle_data(((Server*)events[i].data.ptr)->getFd(), ((Server*)events[i].data.ptr)->GetPort(), httpConf.servers) == 0) {
                    close(((Server*)events[i].data.ptr)->getFd());
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ((Server*)events[i].data.ptr)->getFd(), NULL);
                    delete(((Server*)events[i].data.ptr));
                }
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
