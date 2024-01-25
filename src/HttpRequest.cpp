#include <iostream>
#include <string>
#include <unordered_map>

struct HTTPRequest {
    std::string method;
    std::string path;
    std::string http_version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HTTPRequestParser {
public:
    explicit HTTPRequestParser(const std::string& raw_request) : raw_request(raw_request) {}

    HTTPRequest parse() {
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
            request.body = raw_request.substr(pos + 4);  // Add 4 to skip the "\r\n\r\n"
        }
        return request;
    }

private:
    std::string raw_request;
};

int main() {
    std::string raw_request = "POST /submit_form HTTP/1.1\r\n"
                         "Host: www.example.com\r\n"
                         "User-Agent: Mozilla/5.0\r\n"
                         "Accept: text/html\r\n"
                         "Content-Type: application/x-www-form-urlencoded\r\n"
                         "Content-Length: 23\r\n"
                         "\r\n"
                         "username=example&password=secret";
    HTTPRequestParser parser(raw_request);
    HTTPRequest parsed_request = parser.parse();
    std::cout << "Method: " << parsed_request.method << std::endl;
    std::cout << "Path: " << parsed_request.path << std::endl;
    std::cout << "HTTP Version: " << parsed_request.http_version << std::endl;
    std::cout << "Headers:" << std::endl;
	for (std::unordered_map<std::string, std::string>::const_iterator it = parsed_request.headers.begin(); it != parsed_request.headers.end(); it++) {
		std::cout << " " << it->first << ": " << it->second << std::endl;
	}
    std::cout << "Body: " << parsed_request.body << std::endl;
    return 0;
}