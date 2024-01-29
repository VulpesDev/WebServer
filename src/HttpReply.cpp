/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReply.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 02:10:44 by mcutura           #+#    #+#             */
/*   Updated: 2023/11/03 02:10:44 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

void HTTPResponse::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HTTPResponse::setBody(const std::string& body_content) {
    body = body_content;
    headers["Content-Length"] = std::to_string(body.length());
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
		<< "<p>The requested URL was not found on this server.</p>\r\n"
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
