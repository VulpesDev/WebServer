#include "HandleData.hpp"

/**
 * Process CGI if path is a PHP file and method is GET or POST.
 *
 * @param server the server object
 * @param req the HTTP request object
 * @param response the HTTP response object
 * @param location the location object
 *
 * @return empty string or CGI response
 *
 * @throws None
 */
std::string process_CGI(Server server, HttpRequest req, HTTPResponse response, Location location) {
    if (req.getPath().find(".php") != std::string::npos && (req.getMethod() == "GET" || req.getMethod() == "POST")) {
        CGI cgi(req, location, server);
        if (!check_method_access(server, req.getPath(), "GET")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        int read_fd = cgi.execute_CGI(req,location, server);
        if (read_fd == -1) {
            return (check_error_page(server, req.getPath(), 504));
        }
        else {
            std::cerr << "CGI HANDLING" << std::endl;
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

    std::string check_request = handle_request_checks(server, req);
    if (!check_request.empty())
        return check_request;
    std::string check_cgi = process_CGI(server, req, response, location);
    if ( !check_cgi.empty() ) {
        return check_cgi;
    }
    
    if (req.getMethod() == "GET") {
        std::cerr << "GET REQUEST" << std::endl;
        if (!check_method_access(server, req.getPath(), "GET")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_get_request(server, req.getPath()));
    }
    else if (req.getMethod() == "POST") {
        if (!check_method_access(server, req.getPath(), "POST")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_post_request(server, req.getPath(), req.getBody()));
    }
    else if (req.getMethod() == "DELETE") {
        if (!check_method_access(server, req.getPath(), "DELETE")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_delete_request(server, req.getPath()));
    }
    return "";
}

void handle_data(int client_fd, std::string port, std::vector<Server> serverconfs) {

    Server                          server;
    std::string                     processed_responce;
    std::pair<std::string, ssize_t> received_info = receive_all(client_fd, port, serverconfs, server);
    std::string                     received_data = received_info.first;
    ssize_t                         total_bytes_received = received_info.second;

    try {
        processed_responce = process_request(&received_data[0], total_bytes_received, server);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    send(client_fd, processed_responce.c_str(), processed_responce.length(), 0);
    close(client_fd);
}