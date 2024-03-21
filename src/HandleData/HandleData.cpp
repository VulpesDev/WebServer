
#include "HandleData.hpp"

class HTTPResponse;

/// @brief checks the method being passed and calls the functions accordingly
/// @param request the raw request
/// @param bytes_received number of bytes received
/// @return the raw response message

	//search for '.' with find and save pos then check with substring for .php
	//check for access permisiion then 
	// if (req.getPath().find(".php") != std::string::npos && (req.getMethod() == "GET" || req.getMethod() == "POST")) {
	// 	std::cout << "need to handle cgi file." << std::endl;
	// 	CGI cgi(req, "./data/www/");
	// 	// int cgi_return;
	// 	// std::cout << "checking with execute_CGI" << std::endl;
	// 	int read_fd = cgi.execute_CGI(req, "./data/www/basic.php");
	// 	if (read_fd == -1) {
	// 		generate_error_page(404);
	// 		return "";
	// 	}
	// 	else {
	// 		std::cerr << "CGI HANDLING" << std::endl; //debug
	// 		return (response.send_cgi_response(cgi, req));
	// 		// std::string cgi_result = 
    //         // response.send_cgi_response(cgi, req);
    //         // std::string result = response.getRawResponse();
	// 		// std::cerr << result << std::endl; //debug
    //         // return result;


	// 	} 
	// 	// if((cgi_return = send_cgi_response(cgi, req))){
	// 	// 	return (generate_error_page(cgi_return))
	// 		//assuming there is only one client
	// 		//if needed handle multiple clients....
	// 	// }
	// }

bool    check_method_access(Server server, std::string path, std::string method) {

    // std::cerr << "server vaues: " << std::endl;
    // server.printValues();
    // std::cerr << "------------------" << std::endl;

    std::cerr << "Checking method access, locations size: " << server.locations.size() << std::endl;
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        std::cerr << "Location path: " << it->getPath() << " |path: " << path << std::endl;
        if (it->getPath() == path) {
            std::cerr << "Path match" << std::endl;
            std::vector<std::string> accepted_methods = it->getAcceptedMethods();

            if (accepted_methods.size() == 0) {
                std::cerr << "No accepted methods" << std::endl;
                return false;
            }
            std::cerr << "Accepted methods: ";
            for (std::vector<std::string>::const_iterator accepted_method = accepted_methods.begin();
                 accepted_method != accepted_methods.end();
                 ++accepted_method) {
                std::cerr << *accepted_method << " ";
            }
            std::cerr << std::endl;

            for (std::vector<std::string>::const_iterator accepted_method = accepted_methods.begin();
                 accepted_method != accepted_methods.end();
                 ++accepted_method) {
                std::cerr << "Accepted method: " << *accepted_method << " |method: " << method << std::endl;
                if ((*accepted_method) == method) {
                    std::cerr << "METHOD MATCH" << std::endl;
                    return true;
                }
            }
            return false;
        }
    }
    return true;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        return "";
    }
}

std::string check_error_page(Server server, std::string path, int error_code) {
    HTTPResponse resp(error_code);
    errPages_arr err_pages = server.GetErrPages();
    for (errPages_arr::const_iterator it = err_pages.begin(); it != err_pages.end(); ++it) {
        for (std::vector<int>::const_iterator err_code = it->errs.begin(); err_code != it->errs.end(); ++err_code) {
            if ((*err_code) == error_code) {
                std::string error_page = readFile(it->path);
                if (error_page != "") {
                    resp.setBody(error_page);
                    return resp.getRawResponse();
                }
            }
        }
    }
    resp.setBody(generate_error_page(error_code));
    return resp.getRawResponse();
}

std::string check_redirection(Server server, std::string path) {
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == path && !it->getRedirection().text.empty()) {
            HTTPResponse resp(it->getRedirection().status);
            resp.setHeader("Location", it->getRedirection().text);
            return resp.getRawResponse();
        }
    }
    return "";
}

void    check_directory_index(Server server, HttpRequest& req) {
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == req.getPath() && !it->getIndexFile().empty()) {
            req.setPath(req.getPath() + it->getIndexFile());
            std::cerr << "NEW PATH: " << req.getPath() << std::endl;
            return;
        }
    }
}

std::string process_request(char* request, size_t bytes_received, Server server) {
    HttpRequest req(request, bytes_received);
    HTTPResponse response;
    Location location;

    std::cerr << "Handling request" << std::endl; //debug
    if (req.getHttpVersion() != "HTTP/1.1"){
        return (check_error_page(server, req.getPath(), 505));
    }
    check_directory_index(server, req);
	if (req.getPath().find(".php") != std::string::npos && (req.getMethod() == "GET" || req.getMethod() == "POST")) {
        std::cerr << "CGI REQUEST" << std::endl;
        CGI cgi(req, location);
        std::cerr << "CGI Instance" << std::endl;
        if (!check_method_access(server, req.getPath(), "GET")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        int read_fd = cgi.execute_CGI(req,location);
        if (read_fd == -1) {
            return (check_error_page(server, req.getPath(), 403));
        }
        else {
            std::cerr << "CGI HANDLING" << std::endl;
            return (response.send_cgi_response(cgi, req));
        }
    }
    std::string check_redir = check_redirection(server, req.getPath());
    if (!check_redir.empty()) {
        return check_redir;
    }
    else if (req.getMethod() == "GET") {
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

std::pair<std::string, ssize_t> receive_all(int client_fd) {
    std::string received_data;
    char buffer[MAX_CHUNK_SIZE];
    ssize_t bytes_received;
    ssize_t total_bytes_received = 0;
    ssize_t header_bytes_received = 0;
    ssize_t content_length;

    while (true) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return std::make_pair("", -1); // Error occurred
        } else if (bytes_received == 0) {
            std::cerr << "CLOSED CONNECTION" << std::endl;
            return std::make_pair("", -1); // Connection closed
        }
        // std::cerr << "Adding to received_data" << std::endl;
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);

        header_bytes_received = received_data.find("\r\n\r\n");
        if (header_bytes_received != std::string::npos) {
            // End of received_data found, break the loop
            break;
        }
    }
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
            return std::make_pair("", -1); // Error occurred
        }
    }
    while ((total_bytes_received - header_bytes_received - 4) < content_length) {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            return std::make_pair("", -1); // Error occurred
        }
        if (bytes_received == 0) {
            // Connection closed by client
            std::cerr << "CLOSED CONNECTION" << std::endl;
            break;
        }
        total_bytes_received += bytes_received;
        received_data.append(buffer, bytes_received);
    }
    return std::make_pair(received_data, total_bytes_received);
}

void handle_data(epoll_data_t data) {
    Server server = *(Server *)data.ptr;
    int client_fd = server.getFd();
    std::cerr << "clent_fd: " << client_fd << std::endl;

    std::pair<std::string, ssize_t> received_info = receive_all(client_fd);
    std::string received_data = received_info.first;
    ssize_t total_bytes_received = received_info.second;

    // Process the received data
    // std::cerr << "Total bytes received: " << total_bytes_received << std::endl;
    // std::cerr << "Received data: "<< std::endl;
    // std::cerr.write(&received_data[0], total_bytes_received);
    // std::cerr << std::endl;
    // std::cerr << "END Received data ----------------"<< std::endl;
    
    std::string processed_responce;
    try {
        processed_responce = process_request(&received_data[0], total_bytes_received, server);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    //std::cerr << "Processed responce: " << std::endl << processed_responce << std::endl; //debug
    send(client_fd, processed_responce.c_str(), processed_responce.length(), 0);
    close(client_fd);
}