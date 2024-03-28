#include "HandleData.hpp"

static bool match_vector(const std::string& str, const std::vector<std::string>& vec) {
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        if (*it == str) {
            return true;
        }
    }
    return false;
}

std::string substringUntilNewline(const std::string& str, size_t start_index) {
    size_t end_index = str.find("\r\n", start_index); // Find the index of "\n\r" starting from start_index
    if (end_index == std::string::npos) { // If "\r\n" is not found
        return str.substr(start_index); // Return substring from start_index to end of the string
    } else {
        return str.substr(start_index, end_index - start_index); // Return substring from start_index to end_index
    }
}

std::string receive_headers(int client_fd, ssize_t& total_bytes_received, ssize_t& header_bytes_received) {
    std::string received_data;
    ssize_t     bytes_received = 0;
    char        buffer[MAX_CHUNK_SIZE];
    
    // Receive data until the end of the headers is found
    while (true) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return "";
        } else if (bytes_received == 0) {
            std::cerr << "CLOSED CONNECTION" << std::endl;
             // Connection closed
             return "";
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);

        header_bytes_received = received_data.find("\r\n\r\n");
        if (header_bytes_received != std::string::npos) {
            // End of received_data found, break the loop
            break;
        }
    }
    return received_data;
}

std::string receive_body(int client_fd, ssize_t& total_bytes_received, ssize_t header_bytes_received, std::string recv_data) {
    ssize_t     bytes_received = 0;
    ssize_t     content_length = 0;
    char        buffer[MAX_CHUNK_SIZE];
    std::string received_data(recv_data);

    // Check if Content-Length header is present
    size_t pos = received_data.find("Content-Length: ");
    if (pos == std::string::npos) {
        std::cerr << "Content-Length header not found" << std::endl;
        content_length = 0;
    }
    else {
        try {
            content_length = std::stoll(received_data.substr(pos + 16));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            return ""; // Error occurred
        }
    }
    
    // Receive the body
    while ((total_bytes_received - header_bytes_received - 4) < content_length) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return ""; // Error occurred
        }
        if (bytes_received == 0) {
            // Connection closed by client
            std::cerr << "CLOSED CONNECTION" << std::endl;
            break;
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);
    }
    return received_data;
}

Server get_correct_server_conf(std::string received_data, std::vector<Server> server_confs, std::string port) {
    std::string host_name;
    Server      serv_return;
    size_t      pos = received_data.find("Host: ");

    // Find the host name in the received data
    if (pos == std::string::npos) {
        std::cerr << "Host header not found" << std::endl;
        host_name = "";
    }
    else {
        try {
            host_name = substringUntilNewline(received_data, pos + 6);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            return serv_return; // Error occurred
        }
    }

    // Find the server configuration that matches the host name
    bool default_set = false;
    Server* temp_serv = new Server();
    for (std::vector<Server>::const_iterator it = server_confs.begin(); it != server_confs.end(); ++it) {
        if (it->GetPort() == port && (!default_set || match_vector(host_name, it->GetServNames()))) {
            delete(temp_serv);
            temp_serv = new Server(*it);
            default_set = true;
        }
    }
    serv_return = *temp_serv;
    delete(temp_serv);
    
    return serv_return;
}

std::pair<std::string, ssize_t> receive_all(int client_fd, std::string port, std::vector<Server> server_confs, Server& server) {
    std::string received_data;
    ssize_t     total_bytes_received = 0;
    ssize_t     header_bytes_received = 0;

    received_data = receive_headers(client_fd, total_bytes_received, header_bytes_received);
    if (received_data == "") {
        return std::make_pair("", -1); // Error occurred
    }
    
    server = get_correct_server_conf(received_data, server_confs, port);

    received_data = receive_body(client_fd, total_bytes_received, header_bytes_received, received_data);
    if (received_data == "") {
        return std::make_pair("", -1); // Error occurred
    }

    return std::make_pair(received_data, total_bytes_received);
}