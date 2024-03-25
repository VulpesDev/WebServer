/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtimsina <rtimsina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:23:11 by tvasilev          #+#    #+#             */
/*   Updated: 2024/03/21 20:49:56 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

    HttpRequest::HttpRequest(char *raw_request, size_t len) {
        this->raw_request = std::string (raw_request, len);
        parse();
    }
    
    void HttpRequest::parse() {
        size_t pos = raw_request.find("\r\n\r\n");
        if (pos != std::string::npos) {
            std::string request_line = raw_request.substr(0, raw_request.find("\r\n"));
            size_t first_space = request_line.find(' ');
            size_t second_space = request_line.find(' ', first_space + 1);
            
            this->method = request_line.substr(0, first_space);
            this->path = request_line.substr(first_space + 1, second_space - first_space - 1);
            this->http_version = request_line.substr(second_space + 1);
            std::cerr << "Request line: " << method << "_" << path << "_" << http_version << std::endl;
            
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
                    this->headers[key] = value;
                }
                header_end += 2;  // Move past "\r\n"
				header_start = header_end;
            }
            size_t  body_size = 0;
                /* code */
            if (this->headers.find("Content-Length") == this->headers.end()) {
                std::cerr << "ERRPR" << std::endl;
            }
            else {
                std::string s = this->headers.find("Content-Length")->second;
                body_size = std::stoul(s);// - 1;
            }
            std::cerr << "This is body size in HttpRequest parse(): " << body_size << std::endl; //debug
            // if (body_size > 0) {
            //    if (pos != std::string::npos && pos < raw_request.size()) {
            //         // Calculate body size accurately
            //         size_t bs = std::min(raw_request.size() - pos, body_size);
            //         // Construct std::string from pointer and size
            //         std::string b(&raw_request[pos], bs);
            //         // Store the constructed string
            //         this->body = b;
            //         std::cerr << "Body: " << body << std::endl;
            //     } else {
            //         // Handle the case where pos is out of bounds
            //         std::cerr << "Error: Invalid position for body" << std::endl;
            //     }
            // }
            if (pos + 4 + body_size <= raw_request.size()) {
                std::string body = raw_request.substr(pos + 4, body_size);
                this->body = body;
                // std::cerr << "Body: " << body << std::endl;
            } else {
                std::cerr << "Error: Invalid position for body" << std::endl;
            }
        }
    }

    /// GETTERS ///

    const std::string& HttpRequest::getRawRequest() const { return raw_request; }
    const std::string& HttpRequest::getMethod() const { return method; }
    const std::string& HttpRequest::getPath() const { return path; }
    const std::string& HttpRequest::getHttpVersion() const { return http_version; }
    const std::unordered_map<std::string, std::string>& HttpRequest::getHeaders() const { return headers; }
    const std::string& HttpRequest::getBody() const { return body; }
    std::string HttpRequest::get_query() {
        std::cerr << "get_query" << std::endl;
        size_t i = path.find_first_of('?', 0);
        std::cerr << "get_query seg" << std::endl;
        if (i == std::string::npos) {
            return "";
        }
        std::cerr << "get_query segfault" << std::endl;
        return path.substr(i + 1);
    }

    /// SETTERS ///
    void    HttpRequest::setPath(std::string str) {
        path = str;
    }