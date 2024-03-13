
#include "../../include/HandleData.hpp"

class HTTPResponse;

/// @brief checks the method being passed and calls the functions accordingly
/// @param request the raw request
/// @param bytes_received number of bytes received
/// @return the raw response message
std::string process_request(char* request, size_t bytes_received) {
    HTTPRequestParser p(request, bytes_received);
    HTTPRequest req = p.parse();
    HTTPResponse response;
	//search for '.' with find and save pos then check with substring for .php
	//check for access permisiion then 
	if (req.path.find(".php") != std::string::npos && (req.method == "GET" || req.method == "POST")) {
		std::cout << "need to handle cgi file." << std::endl;
		CGI cgi(req, "./data/www/");
		// int cgi_return;
		// std::cout << "checking with execute_CGI" << std::endl;
		int read_fd = cgi.execute_CGI(req, "./data/www/basic.php");
		if (read_fd == -1) {
			generate_error_page(404);
			return "";
		}
		else {
			std::cerr << "CGI HANDLING" << std::endl; //debug
			return (response.send_cgi_response(cgi, req));
			// std::string cgi_result = 
            // response.send_cgi_response(cgi, req);
            // std::string result = response.getRawResponse();
			// std::cerr << result << std::endl; //debug
            // return result;


		} 
		// if((cgi_return = send_cgi_response(cgi, req))){
		// 	return (generate_error_page(cgi_return))
			//assuming there is only one client
			//if needed handle multiple clients....
		// }
	}
    else if (req.method == "GET") {
        std::cerr << "GET REQUEST" << std::endl; //debug
 
        return (handle_get_request(req.path));
    }
    else if (req.method == "POST") {
        std::cerr << "POST REQUEST" << std::endl; //debug
        return (handle_post_request(req.path, req.body));
    }
    else if (req.method == "DELETE") {
        std::cerr << "DELETE REQUEST" << std::endl; //debug
        return (handle_delete_request(req.path));
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

void handle_data(int client_fd) {
    std::pair<std::string, ssize_t> received_info = receive_all(client_fd);
    std::string received_data = received_info.first;
    ssize_t total_bytes_received = received_info.second;

    // Process the received data
    // std::cerr << "Total bytes received: " << total_bytes_received << std::endl;
    // std::cerr << "Received data: "<< std::endl;
    // std::cerr.write(&received_data[0], total_bytes_received);
    // std::cerr << std::endl;
    // std::cerr << "END Received data ----------------"<< std::endl;
    
    std::string processed_req;
    try {
        processed_req = process_request(&received_data[0], total_bytes_received);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    std::cerr << "Processed request: " << processed_req << std::endl; //debug
    send(client_fd, processed_req.c_str(), processed_req.length(), 0);
    close(client_fd);
}