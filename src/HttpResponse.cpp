/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 02:10:44 by mcutura           #+#    #+#             */
/*   Updated: 2024/03/13 23:58:25 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>
#include <CGI.hpp>
#include "../include/HttpMessage.hpp"
#include "../include/CGI.hpp"

static void set_signal_kill_child_process(int sig)
{
	(void) sig;
    kill(-1,SIGKILL);
}

void HTTPResponse::handle_cgi_get_response(HTTPResponse &resp, std::string& cgi_ret) {
	std::stringstream ss(cgi_ret);
	std::cerr << "---------cgi_ret----	" << cgi_ret << std::endl;
	size_t	temp_i;
	std::string tmp;
	std::string body;

	resp.setHeader("Server", "Spyder");
	resp.setHeader("Connection", "close");
	while (getline(ss, tmp, '\n'))
	{
		if (tmp.length() == 1 && tmp[0] == '\r')
			break;
		size_t	mid_delim = tmp.find(":");
		size_t	end_delim = tmp.find("\n");
		if (tmp[end_delim] == '\r') {
			tmp.erase(tmp.length() - 1, 1);
			end_delim -= 1;
		}
		//remove trailing semicolon
		if ((temp_i = tmp.find(";")) != std::string::npos) {
			tmp = tmp.substr(0, temp_i);
		}
		std::string key = tmp.substr(0, mid_delim);
		std::string value = tmp.substr(mid_delim + 1, end_delim);
		resp.setHeader(key, value);
	}
	while (getline(ss, tmp, '\n')) {
		body += tmp;
		body += "\n";
	}
	std::cerr << "this is body of  handle_cgi_get_response: " << body << std::endl;
	resp.setBody(body);
	resp.setHeader("Content-Length", std::to_string(body.size()));
	// return resp.getRawResponse();
	
}

void HTTPResponse::handle_cgi_post_response(HTTPResponse& resp, std::string& cgi_ret) {
	std::stringstream ss(cgi_ret);
	size_t	temp_i;
	std::string tmp;
	std::string body;

	resp.setHeader("Server", "Spyder");
	resp.setHeader("Connection", "close");
	while (getline(ss, tmp, '\n'))
	{
		if (tmp.length() == 1 && tmp[0] == '\r')
			break;
		size_t	mid_delim = tmp.find(":");
		size_t	end_delim = tmp.find("\n");
		if (tmp[end_delim] == '\r') {
			tmp.erase(tmp.length() - 1, 1);
			end_delim -= 1;
		}
		//remove trailing semicolon
		if ((temp_i = tmp.find(";")) != std::string::npos) {
			tmp = tmp.substr(0, temp_i);
		}
		std::string key = tmp.substr(0, mid_delim);
		std::string value = tmp.substr(mid_delim + 1, end_delim);
		resp.setHeader(key, value);
	}
	while (getline(ss, tmp, '\n')) {
		body += tmp;
		body += "\n";
	}
	// std::string full_path = "./data/cgi-bin" + request.path; request is empty
	std::string full_path = "./data/cgi-bin";
	//if httprequest demands to create folder and set different path 
	//then should handle making folder and updating path.
	FILE *fp = fopen(full_path.c_str(), "w");
	if (!fp) {
		generate_error_page(500);
		return ;
	}
	fwrite(body.c_str(), body.size(), 1, fp);
	fclose(fp);

	resp.setHeader("Content-Length", std::to_string(body.size()));
}

std::string HTTPResponse::send_cgi_response(CGI& cgi_handler, HttpRequest request) {
	int fd[2];
	fd[0] = cgi_handler.get_read_fd();
	fd[1] = cgi_handler.get_write_fd();

	std::cout << "CGI send_cgi_response.\n";
	if (fd[0] == -1 || fd[1] == -1) {
		std::cout << "cgi response build failed" << std::endl;
		signal(SIGALRM, set_signal_kill_child_process);
		alarm(30);
		signal(SIGALRM, SIG_DFL);
		close(fd[0]);
		close(fd[1]);
		generate_error_page(500);
		return NULL;
	}
	std::cout << "CGI send_cgi_response fd are good.\n";
	// cgi_handler.write_to_CGI();
	std::cout << "CGI send_cgi_response write to cgi finished.\n";
	close(fd[1]);
	std::string cgi_ret = cgi_handler.read_from_CGI(fd[0]);
	
	std::cout << "\n\nthis is in cgi_ret from send_cgi_response: " << cgi_ret << "----" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	if (cgi_ret.empty()) {
		generate_error_page(500);
		return 0;
	}
	close(fd[0]);
	std::cout << "CGI read succes.\n";
	if (cgi_ret.compare("cgi: failed") == 0) {
		generate_error_page(400);
		return 0;
	} else {
		HTTPResponse resp(200);
		if (request.getMethod() == "GET") {
			std::cout << "CGI get response" << std::endl;
			// resp.handle_cgi_get_response(resp, cgi_ret);
			handle_cgi_get_response(resp, cgi_ret);
		}
		else if (request.getMethod() == "POST") {
			handle_cgi_post_response(resp, cgi_ret);
		}
		std::string result = resp.getRawResponse();
		return result;
	}
	return "";
}

#include <ctime>
/// @brief get_time uses std::time to display system time
/// @return the date and time as a std::string
std::string get_time() {
    char buffer[80];
    std::time_t current_time = std::time(nullptr);
    struct tm* time_info = std::localtime(&current_time);
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", time_info);
    return (buffer);
}

#include <sys/utsname.h>
HTTPResponse::HTTPResponse(int status_code)
        : status_code(status_code){
			reason_phrase = get_status_message(status_code);
			this->setHeader("Date", get_time());
			struct utsname info;
			if (uname(&info) != 0) {
				throw std::runtime_error("Failed to retruve system information");
			}
			this->setHeader("Server", info.sysname + std::string("/") + info.release
				+ std::string(" (") + info.machine + std::string(")"));
		}

void HTTPResponse::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HTTPResponse::setBody(const std::string& body_content) {
    body = body_content;
	this->setHeader("Content-Length", std::to_string(body.length()));
}

std::string HTTPResponse::getRawResponse() const {
    std::string raw_response;
    raw_response += "HTTP/1.1 " + std::to_string(status_code) + " " + reason_phrase + "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        raw_response += it->first + ": " + it->second + "\r\n";
    }
    raw_response += "\r\n" + body;
    return raw_response;
}

std::string const generate_error_page(int status)
{
	std::ostringstream	ss;
	std::string const	msg = get_status_message(status);

	ss << "<!DOCTYPE html>\r\n"
		<< "<html>\r\n"
		<< "<head>\r\n"
		<< "<title>" << status << " " << msg << "</title>\r\n"
		<< "</head>\r\n"
		<< "<body>\r\n"
		<< "<h1>" << msg << "</h1>\r\n"
	// TODO: Generate explanation for other common error codes or remove this p
		<< "<p>" << get_status_message_detail(status) << "</p>\r\n"
		<< "</body>\r\n"
		<< "</html>\r\n";
	return ss.str();
}

std::string const get_status_message(int status)
{
	switch (status) {
		// 1xx Information responses
		case 100:
			return "Continue";
		case 101:
			return "Switching Protocols";
		case 102:
			return "Processing";
		case 103:
			return "Early Hints";
		// 2xx Succesful responses
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "No Content";
		case 205:
			return "Reset Content";
		case 206:
			return "Partial Content";
		case 207:
			return "Multi-Status";
		case 208:
			return "Already Reported";
		case 226:
			return "IM Used";
		// 3xx Redirection messages
		case 300:
			return "Multiple Choices";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		// case 305: return "Use Proxy";  // DEPRECATED
		// case 306: return "";  // no longer unused, but reserved
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		// 4xx Client error responses
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 402:
			return "Payment Required";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 406:
			return "Not Acceptable";
		case 407:
			return "Proxy Authentication Required";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 411:
			return "Length Required";
		case 412:
			return "Precondition Failed";
		case 413:
			return "Content Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 416:
			return "Range Not Satisfiable";
		case 417:
			return "Expectation Failed";
		case 418:
			return "I'm a teapot";
		case 421:
			return "Misdirected Request";
		case 422:
			return "Unprocessable Content";
		case 423:
			return "Locked";
		case 424:
			return "Failed Dependency";
		case 425:
			return "Too Early";
		case 426:
			return "Upgrade Required";
		case 428:
			return "Precondition Required";
		case 429:
			return "Too Many Requests";
		case 431:
			return "Request Header Fields Too Large";
		case 451:
			return "Unavailable For Legal Reasons";
		// 5xx Server error responses
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		case 506:
			return "Variant Also Negotiates";
		case 507:
			return "Insufficient Storage";
		case 508:
			return "Loop Detected";
		case 510:
			return "Not Extended";
		case 511:
			return "Network Authentication Required";
		default:
			return "";
	}
}

std::string const get_status_message_detail(int status) {
    switch(status) {
        case 100:
            return "The server has received the request headers and the client should proceed to send the request body.";
        case 101:
            return "The requester has asked the server to switch protocols.";
        case 102:
            return "The server is processing the request, but no response is available yet.";
        case 200:
            return "OK";
        case 201:
            return "The request has been fulfilled and resulted in a new resource being created.";
        case 202:
            return "The request has been accepted for processing, but the processing has not been completed.";
        case 203:
            return "The server successfully processed the request but is returning information from another source.";
        case 204:
            return "The server successfully processed the request but is not returning any content.";
        case 205:
            return "The server successfully processed the request but is not returning any content. The client should reset the document view.";
        case 206:
            return "The server is delivering only part of the resource due to a range header sent by the client.";
        case 207:
            return "The message body that follows is an XML message and can contain a number of separate response codes, depending on how many sub-requests were made.";
        case 300:
            return "The requested resource has multiple representations.";
        case 301:
            return "The requested resource has been assigned a new permanent URI.";
        case 302:
            return "The requested resource resides temporarily under a different URI.";
        case 303:
            return "The response to the request can be found under a different URI.";
        case 304:
            return "Indicates that the resource has not been modified since the version specified by the request headers.";
        case 305:
            return "The requested resource must be accessed through the proxy given by the Location field.";
        case 307:
            return "The requested resource resides temporarily under a different URI.";
        case 308:
            return "The target resource has been assigned a new permanent URI and any future references to this resource ought to use one of the enclosed URIs.";
        case 400:
            return "The server could not understand the request due to invalid syntax.";
        case 401:
            return "The requested resource requires authentication.";
        case 402:
            return "This code is reserved for future use.";
        case 403:
            return "The server understood the request, but refuses to authorize it.";
        case 404:
            return "The requested URL was not found on this server.";
        case 405:
            return "The method specified in the request is not allowed.";
        case 406:
            return "The resource identified by the request is only capable of generating response entities that have content characteristics not acceptable according to the accept headers sent in the request.";
        case 407:
            return "The client must first authenticate itself with the proxy.";
        case 408:
            return "The server timed out waiting for the request.";
        case 409:
            return "Indicates that the request could not be completed due to a conflict with the current state of the resource.";
        case 410:
            return "Indicates that the requested resource is no longer available and will not be available again.";
        case 411:
            return "The server refuses to accept the request without a defined Content-Length.";
        case 412:
            return "The precondition given in one or more of the request-header fields evaluated to false.";
        case 413:
            return "The server is refusing to process a request because the request entity is larger than the server is willing or able to process.";
        case 414:
            return "The server is refusing to service the request because the Request-URI is longer than the server is willing to interpret.";
        case 415:
            return "The server is refusing to service the request because the entity of the request is in a format not supported by the requested resource for the requested method.";
        case 416:
            return "The client has asked for a portion of the file, but the server cannot supply that portion.";
        case 417:
            return "The server cannot meet the requirements of the Expect request-header field.";
        case 418:
            return "The server refuses to brew coffee because it is, permanently, a teapot.";
        case 421:
            return "The request was directed at a server that is not able to produce a response.";
        case 422:
            return "The request was well-formed but was unable to be followed due to semantic errors.";
        case 423:
            return "The resource that is being accessed is locked.";
        case 424:
            return "The request failed due to failure of a previous request.";
        case 426:
            return "The client should switch to a different protocol.";
        case 428:
            return "The origin server requires the request to be conditional.";
        case 429:
            return "The user has sent too many requests in a given amount of time.";
        case 431:
            return "The server is unwilling to process the request because its header fields are too large.";
        case 451:
            return "The server is denying access to the resource as a consequence of a legal demand.";
        case 500:
            return "The server encountered an unexpected condition.";
        case 501:
            return "The server does not support the functionality required to fulfill the request.";
        case 502:
            return "The server received an invalid response from the upstream server.";
        case 503:
            return "The server is currently unable to handle the request.";
        case 504:
            return "The server did not receive a timely response from the upstream server.";
        case 505:
            return "The server does not support the HTTP protocol version that was used in the request.";
        case 506:
            return "Transparent content negotiation for the request results in a circular reference.";
        case 507:
            return "The server is unable to store the representation needed to complete the request.";
        case 508:
            return "The server detected an infinite loop while processing the request.";
        case 510:
            return "Further extensions to the request are required for the server to fulfill it.";
        case 511:
            return "The client needs to authenticate to gain network access.";
        case 599:
            return "This status code is not specified in any RFC and is used by some HTTP proxies to signal a network connect timeout error.";
        default:
            return "Unknown Status: An unknown status code was encountered.";
    }
}

// int main() {
//     HTTPResponse response(200, get_status_message(200));
//     response.setHeader("Date", "Mon, 25 Jan 2024 12:00:00 GMT");
//     response.setHeader("Server", "Apache/2.4.41 (Unix)");
//     response.setHeader("Content-Type", "text/html");
//     response.setBody("<html><body>OK</body></html>");

//     std::string raw_response = response.getRawResponse();
//     std::cout << raw_response << std::endl;

//     return 0;
// }
