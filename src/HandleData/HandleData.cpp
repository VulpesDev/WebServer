
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

    std::cerr << "server vaues: " << std::endl;
    server.printValues();
    std::cerr << "------------------" << std::endl;

    std::cerr << "Checking method access, locations size: " << server.locations.size() << std::endl;
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        std::cerr << "Location path: " << it->getPath() << " |path: " << path << std::endl;
        if (it->getPath() == path) {
            std::cerr << "Path match" << std::endl;
            if (it->getAcceptedMethods().size() == 0) {
                std::cerr << "No accepted methods" << std::endl;
                return true;
            }
            for (std::vector<std::string>::const_iterator accepted_method = it->getAcceptedMethods().begin();
                 accepted_method != it->getAcceptedMethods().end();
                 ++accepted_method) {
                    std::cerr << "Accepted method: " << *accepted_method << " |method: " << method << std::endl;
                if ((*accepted_method) == method) {
                    return true;
                }
            }
        }
    }
    return false;
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

    for (errPages_arr::const_iterator it = server.GetErrPages().begin(); it != server.GetErrPages().end(); ++it) {
        for (std::vector<int>::const_iterator err_code = it->errs.begin(); err_code != it->errs.end(); ++err_code) {
            if ((*err_code) == error_code) {
                std::string error_page = readFile(it->path);
                if (error_page != "") {
                    return error_page;
                }
                //return check_error_page(data, path, 404);
            }
        }
    }
    return generate_error_page(404);
}

std::string process_request(char* request, size_t bytes_received, Server server) {
    HttpRequest req(request, bytes_received);
    HTTPResponse response;

    std::cerr << "Handling request" << std::endl; //debug
    if (req.getMethod() == "GET") {
        std::cerr << "GET REQUEST" << std::endl;
        if (check_method_access(server, req.getPath(), "GET")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_get_request(req.getPath()));
    }
    else if (req.getMethod() == "POST") {
        if (!check_method_access(server, req.getPath(), "POST")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_post_request(req.getPath(), req.getBody()));
    }
    else if (req.getMethod() == "DELETE") {
        if (!check_method_access(server, req.getPath(), "DELETE")) {
            return (check_error_page(server, req.getPath(), 403));
        }
        return (handle_delete_request(req.getPath()));
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
    std::cerr << "Processed responce: " << std::endl << processed_responce << std::endl; //debug
    send(client_fd, processed_responce.c_str(), processed_responce.length(), 0);
    close(client_fd);
}