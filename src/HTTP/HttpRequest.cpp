/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtimsina <rtimsina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 18:23:11 by tvasilev          #+#    #+#             */
/*   Updated: 2024/03/17 17:40:14 by rtimsina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

    HttpRequest::HttpRequest(char *raw_request, size_t len) {
        std::string s(raw_request, len);
        this->raw_request = s;
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
                body_size = std::stoul(s) - 1;
            }
            std::cerr << "body size: " << body_size << std::endl; //debug
            if (body_size > 0) {
                std::string b(&raw_request[pos], body_size);
                this->body = b;
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