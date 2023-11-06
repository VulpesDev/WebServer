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

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---

HttpReply::HttpReply(int status) : AHttpMessage(), 
	status_code_(status)
{
	std::ostringstream	ss;

	ss << this->version_ << " " << status << " "
		<< HttpReply::get_status_message(status) << "\r\n";
	if (ss.fail())
		return ;
	this->start_line_ = ss.str();
	ss.clear();
	ss.str(std::string());
	if (status == 200) {
		get_payload("./data/www/index.html", this->payload_);
	} else {
		this->payload_ = generate_error_page(status);
	}
	ss << "Content-Type: text/html\r\n" \
		<< "Content-Length: " << this->payload_.size() << "\r\n" \
		<< "Connection: Close" << "\r\n";
	if (ss.fail())
		return ;
	this->header_ = ss.str();
	this->raw_ = this->start_line_ + \
				this->header_ + "\r\n" + \
				this->payload_ + "\r\n";
}

HttpReply::~HttpReply()
{}

////////////////////////////////////////////////////////////////////////////////
// --- METHODS ---

// Source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
std::string const HttpReply::get_status_message(int status)
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

////////////////////////////////////////////////////////////////////////////////
// --- INTERNALS ---

void HttpReply::get_payload(std::string const &path, std::string &payload)
{
	std::ifstream	file(path.c_str());
	std::string		line;

	if (!file.is_open()) {
		payload.clear();
		return ;
	}
	while (getline(file, line)) {
		payload += line + "\n";
	}
	file.close();
}

std::string const HttpReply::generate_error_page(int status)
{
	std::ostringstream	ss;
	std::string const	msg = HttpReply::get_status_message(status);

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
