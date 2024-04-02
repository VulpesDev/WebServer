#include "HandleData.hpp"
std::string process_CGI(Server server, HttpRequest req, HTTPResponse response, Location location) {
    
    if (req.getPath().find(".php") != std::string::npos && (req.getMethod() == "GET" || req.getMethod() == "POST")) {

        if (!check_method_access(server, req.getPath(), "GET") || !check_method_access(server, req.getPath(), "POST")) {
            return (check_error_page(server, 403));
        }
        CGI cgi(req, location, server);
        int read_fd = cgi.execute_CGI(req,location, server);
        if (read_fd == -1) {
            return (check_error_page(server, 502));
        }
        else if (read_fd == -2) {
            return (check_error_page(server, 504));
        }
        else {
            return (response.send_cgi_response(cgi, req, server));
        }
    }
    return "";
}

/**
 * Processes the incoming HTTP request and generates an appropriate response.
 *
 * @param request the incoming HTTP request
 * @param bytes_received the number of bytes received in the request
 * @param server the server configuration
 *
 * @return the HTTP response generated for the request
 *
 */
std::string process_request(char* request, size_t bytes_received, Server server) {
    HttpRequest req(request, bytes_received);
    HTTPResponse response;
    Location location;

    std::cout << "Received " << "\033[32m" << req.getMethod() << "\033[0m" << " request for " << "\033[33m" << req.getPath() << "\033[0m" << " on port " << server.GetPort() << std::endl;

    std::string check_request = handle_request_checks(server, req);
    if (!check_request.empty())
        return check_request;
        
    // std::string check_cgi = process_CGI(server, req, response);
    std::string check_cgi = process_CGI(server, req, response, location);
    if ( !check_cgi.empty() ) {
        return check_cgi;
    }
    
    if (req.getMethod() == "GET") {
        if (!check_method_access(server, req.getPath(), "GET")) {
            return (check_error_page(server, 403));
        }
        return (handle_get_request(server, req.getPath()));
    }
    else if (req.getMethod() == "POST") {
        if (!check_method_access(server, req.getPath(), "POST")) {
            return (check_error_page(server, 403));
        }
        return (handle_post_request(server, req.getPath(), req.getBody()));
    }
    else if (req.getMethod() == "DELETE") {
        if (!check_method_access(server, req.getPath(), "DELETE")) {
            return (check_error_page(server, 403));
        }
        return (handle_delete_request(server, req.getPath()));
    }
    return "";
}

int handle_data(int client_fd, std::string port, std::vector<Server> serverconfs) {

    Server                         server;
    std::string                     processed_responce;
    std::pair<std::string, ssize_t> received_info = receive_all(client_fd, port, serverconfs, server);
    std::string                     received_data = received_info.first;
    ssize_t                         total_bytes_received = received_info.second;

    if (total_bytes_received == -1) {
        return 0;
    }
    try {
        processed_responce = process_request(&received_data[0], total_bytes_received, server);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    if (send(client_fd, processed_responce.c_str(), processed_responce.length(), 0) == -1) {
        return 0;
    }
    std::cout << "Response for " << client_fd << " sent to client" << std::endl;
    close(client_fd);
    return 1;
}
