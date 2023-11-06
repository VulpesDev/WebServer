/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 02:10:41 by mcutura           #+#    #+#             */
/*   Updated: 2023/11/03 02:10:41 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

////////////////////////////////////////////////////////////////////////////////
// --- CONSTANTS ---

std::string const HttpRequest::methods[] = { "GET", "POST", "DELETE", "HEAD" };

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(std::string const &raw) : AHttpMessage(raw)
{}

HttpRequest::HttpRequest(HttpRequest const &other) : AHttpMessage(other)
{
	*this = other;
}

HttpRequest const &HttpRequest::operator=(HttpRequest const &rhs)
{
	if(this != &rhs) {
		AHttpMessage::operator=(rhs);
		this->method_ = rhs.method_;
		this->URI_ = rhs.URI_;
	}
	return *this;
}

HttpRequest::~HttpRequest()
{}

////////////////////////////////////////////////////////////////////////////////
// --- METHODS ---

void HttpRequest::append(std::string const &tail)
{
	this->raw_.append(tail);
}

int HttpRequest::validate_start_line()
{
	std::string line;
	std::stringstream ss;

	ss.str(this->raw_);
	// From RFC 2616 #section 4.1
	// "In the interest of robustness, servers SHOULD ignore any empty
	// line(s) received where a Request-Line is expected."
	while (this->start_line_.empty()) {
		if (!std::getline(ss, this->start_line_))
			return 400L;
	}
	// (METHOD) (URL) (HTTP/version)
	std::vector<std::string> vec = ft_splitstr(this->start_line_, " \t\r\n");
	if (vec.size() != 3) {
		return 400L;
	}
	if (vec[2].compare("HTTP/1.0") && vec[2].compare("HTTP/1.1")) {
		return 505L;
	}
	this->version_ = vec[2];
	this->URI_ = vec[1];
	this->method_ = vec[0];
	if (!validate_method())
		return 400L;
	return 0L;
}

bool HttpRequest::validate_method()
{
	for (size_t i = 0; \
		i < sizeof(HttpRequest::methods)/sizeof(HttpRequest::methods[0]); ++i) {
		if (HttpRequest::methods[i] == this->method_)
			return true;
	}
	return false;
}

bool HttpRequest::parse_headers()
{
	typedef std::pair<HeaderMap::iterator, bool> InsertReturn;

	std::istringstream		ss;
	std::string				line;
	std::string::size_type	sep;
	std::string::size_type	head_start = this->raw_.find("\n") + 1;
	std::string::size_type	head_len = this->raw_.find("\r\n\r\n") - head_start;
	HeaderMap::iterator		previous;
	InsertReturn			check;

	this->header_ = this->raw_.substr(head_start, head_len);
	ss.str(this->header_);
	while (std::getline(ss, line)) {
		if ((sep = line.find(":")) == std::string::npos) {
			if (previous == this->headers_.end() || \
				(line.compare(0, 1, " ") && line.compare(0, 1, "\t")))
				return false;
			previous->second.append(line.substr(line.find_first_not_of(" \t")));
			continue;
		}
		head_start = line.find_first_not_of(" \t", sep + 1);
		check = this->headers_.insert( \
				std::make_pair(line.substr(0, sep), line.substr(head_start)));
		// From RFC 2616 #section 4.2
		// Multiple message-header fields with the same field-name MAY be
		// present in a message if and only if the entire field-value for that
		// header field is defined as a comma-separated list
		if (!check.second) {
			check.first->second.append("," + line.substr(head_start));
		}
		previous = check.first;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// --- GETTERS ---

// TODO: expand this check, now is valid only for methods without payload
// if no Content-Length or Transfer-Encoding header, there should be no body
bool HttpRequest::is_complete() const
{
	return (this->raw_.find("\r\n\r\n") != std::string::npos);
}

std::string const &HttpRequest::method() const
{
	return this->method_;
}

std::string	const &HttpRequest::URI() const
{
	return this->URI_;
}

std::string const &HttpRequest::get_header(std::string const &key) const
{
	static const std::string empty = "";
	HeaderMap::const_iterator it = this->headers_.find(key);
	if (it == this->headers_.end()) {
		return empty;
	}
	return it->second;
}
