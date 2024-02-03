/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:23:11 by tvasilev          #+#    #+#             */
/*   Updated: 2024/02/04 00:16:31 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

    HTTPRequestParser::HTTPRequestParser(char *raw_request, size_t len) {
        std::string s(raw_request, len);
        this->raw_request = s;
    }
    
    HTTPRequest HTTPRequestParser::parse() {
        HTTPRequest request;
        size_t pos = raw_request.find("\r\n\r\n");
        if (pos != std::string::npos) {
            std::string request_line = raw_request.substr(0, raw_request.find("\r\n"));
            size_t first_space = request_line.find(' ');
            size_t second_space = request_line.find(' ', first_space + 1);
            
            request.method = request_line.substr(0, first_space);
            request.path = request_line.substr(first_space + 1, second_space - first_space - 1);
            request.http_version = request_line.substr(second_space + 1);
            
            size_t header_start = request_line.length() + 2;  // Add 2 to skip the "\r\n"
            std::string headers_str = raw_request.substr(header_start, pos - header_start);
            size_t header_end = 0;
			header_start = 0;
            while ((header_end = headers_str.find("\r\n", header_end)) != std::string::npos) {
                std::string header_line = headers_str.substr(header_start, header_end - header_start);
                size_t colon_pos = header_line.find(':');
                if (colon_pos != std::string::npos) {
                    std::string key = header_line.substr(0, colon_pos);
                    std::string value = header_line.substr(colon_pos + 2);  // Add 2 to skip the ": "
                    request.headers[key] = value;
                }
                header_end += 2;  // Move past "\r\n"
				header_start = header_end;
            }
            size_t  body_size = 0;
                /* code */
            if (request.headers.find("Content-Length") == request.headers.end()) {
                std::cerr << "ERRPR" << std::endl;
            }
            else {
                std::string s = request.headers.find("Content-Length")->second;
                body_size = std::stoul(s);
            }

            
            std::cerr << "body size: " << body_size << std::endl;
            //request.body = raw_request.substr(pos + 4, body_size);  // Add 4 to skip the "\r\n\r\n"
            std::string b(&raw_request[pos+4], body_size);
            request.body = b;
            std::cerr << "BOOOOOOOODY: ";
            std::cerr.write(&request.body[0], body_size);
            std::cerr << std::endl;
        }
        return request;
    }

// int main() {
//     std::string raw_request = "POST /submit_form HTTP/1.1\r\n"
//                          "Host: www.example.com\r\n"
//                          "User-Agent: Mozilla/5.0\r\n"
//                          "Accept: text/html\r\n"
//                          "Content-Type: application/x-www-form-urlencoded\r\n"
//                          "Content-Length: 23\r\n"
//                          "\r\n"
//                          "username=example&password=secret";
//     HTTPRequestParser parser(raw_request);
//     HTTPRequest parsed_request = parser.parse();
//     std::cout << "Method: " << parsed_request.method << std::endl;
//     std::cout << "Path: " << parsed_request.path << std::endl;
//     std::cout << "HTTP Version: " << parsed_request.http_version << std::endl;
//     std::cout << "Headers:" << std::endl;
// 	for (std::unordered_map<std::string, std::string>::const_iterator it = parsed_request.headers.begin(); it != parsed_request.headers.end(); it++) {
// 		std::cout << " " << it->first << ": " << it->second << std::endl;
// 	}
//     std::cout << "Body: " << parsed_request.body << std::endl;
//     return 0;
// }