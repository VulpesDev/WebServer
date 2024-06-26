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
            }
            else {
                std::string s = this->headers.find("Content-Length")->second;
                char *p;
                body_size = strtoul(s.c_str(), &p, 10);// - 1;
            }
            if (pos + 4 + body_size <= raw_request.size()) {
                std::string body = raw_request.substr(pos + 4, body_size);
                this->body = body;
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
    const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return headers; }
    const std::string& HttpRequest::getBody() const { return body; }
    std::string HttpRequest::get_query() {
        size_t i = path.find_first_of('?', 0);
        if (i == std::string::npos) {
            return "";
        }
        return path.substr(i + 1);
    }

    /// SETTERS ///
    void    HttpRequest::setPath(std::string str) {
        path = str;
    
    }

