#include "HandleData.hpp"

/**
 * Function to check if a string matches any element in a vector of strings.
 *
 * @param str the string to be checked
 * @param vec the vector of strings to be checked against
 *
 * @return true if the string matches any element in the vector, false otherwise
 *
 * @throws None
 */
static bool match_vector(const std::string& str, const std::vector<std::string>& vec) {
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        if (*it == str) {
            return true;
        }
    }
    return false;
}

/**
 * Returns a substring of the given string starting from the specified index until the first occurrence of "\r\n" or the end of the string, whichever comes first.
 *
 * @param str The input string from which to extract the substring.
 * @param start_index The index from which to start extracting the substring.
 *
 * @return The substring of the given string starting from the specified index until the first occurrence of "\r\n" or the end of the string, whichever comes first.
 *
 * @throws None
 */
std::string substringUntilNewline(const std::string& str, size_t start_index) {
    size_t end_index = str.find("\r\n", start_index);
    if (end_index == std::string::npos) {
        return str.substr(start_index);
    } else {
        return str.substr(start_index, end_index - start_index);
    }
}

/**
 * Receive headers from a client socket until the end of the headers is found.
 *
 * @param client_fd The file descriptor of the client socket.
 * @param total_bytes_received The total number of bytes received so far.
 * @param header_bytes_received The number of bytes received in the headers.
 *
 * @return A string containing the received headers data.
 *
 * @throws None
 */
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
             return "";
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);

        header_bytes_received = received_data.find("\r\n\r\n");
        if (header_bytes_received != (long)std::string::npos) {
            // End of received_data found, break the loop
            break;
        }
    }
    return received_data;
}

/**
 * Receives the body of a request from a client.
 *
 * @param client_fd The file descriptor of the client socket.
 * @param total_bytes_received A reference to the total number of bytes received.
 * @param header_bytes_received The number of bytes received for the headers.
 * @param recv_data The data received so far, including the headers.
 *
 * @return The received data, including the headers and the body.
 *
 * @throws std::invalid_argument If the Content-Length header is invalid.
 */
std::string receive_body(int client_fd, ssize_t& total_bytes_received, ssize_t header_bytes_received, std::string recv_data) {
    ssize_t     bytes_received = 0;
    ssize_t     content_length = 0;
    char        buffer[MAX_CHUNK_SIZE];
    std::string received_data(recv_data);

    // Check if Content-Length header is present
    size_t pos = received_data.find("Content-Length: ");
    if (pos == std::string::npos) {
        content_length = 0; 
    }
    else {
        try {
            char* end;
            content_length = strtol(received_data.c_str() + pos + 16, &end, 10);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            return "";
        }
    }
    
    // Receive the body
    while ((total_bytes_received - header_bytes_received - 4) < content_length) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return "";
        }
        if (bytes_received == 0) {
            // Connection closed by client
            break;
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);
    }
    return received_data;
}

/**
 * Retrieves the correct server configuration based on the received data, server configurations, and port.
 *
 * @param received_data The received data containing the host name.
 * @param server_confs The vector of server configurations.
 * @param port The port number.
 *
 * @return The server configuration that matches the host name and port.
 *
 * @throws std::invalid_argument If an invalid argument is provided.
 */
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
            return serv_return;
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

/**
 * A function to receive all data from a client, including headers and body.
 *
 * @param client_fd the file descriptor of the client
 * @param port the port number
 * @param server_confs a vector of server configurations
 * @param server a reference to the Server object to update
 *
 * @return a pair of received data and total bytes received
 *
 * @throws None
 */
std::pair<std::string, ssize_t> receive_all(int client_fd, std::string port, std::vector<Server> server_confs, Server& server) {
    std::string received_data;
    ssize_t     total_bytes_received = 0;
    ssize_t     header_bytes_received = 0;

    received_data = receive_headers(client_fd, total_bytes_received, header_bytes_received);
    if (received_data == "") {
        return std::make_pair("", -1);
    }
    
    server = get_correct_server_conf(received_data, server_confs, port);

    received_data = receive_body(client_fd, total_bytes_received, header_bytes_received, received_data);
    if (received_data == "") {
        return std::make_pair("", -1);
    }

    return std::make_pair(received_data, total_bytes_received);
}
