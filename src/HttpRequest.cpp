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
			return 400;
	}
	// (METHOD) (URL) (HTTP/version)
	std::vector<std::string> vec = ft_splitstr(this->start_line_, " \t\r\n");
	if (vec.size() != 3) {
		return 400;
	}
	if (vec[2].compare("HTTP/1.0") && vec[2].compare("HTTP/1.1")) {
		return 505;
	}
	this->version_ = vec[2];
	this->URI_ = vec[1];
	// TODO: validate method
	this->method_ = vec[0];
	return 0;
}

bool HttpRequest::parse_headers()
{
	typedef std::map<std::string,std::string>::iterator MapIter;
	typedef std::pair<MapIter, bool> InsertReturn;

	std::istringstream		ss;
	std::string				line;
	std::string::size_type	sep;
	std::string::size_type	head_start = this->raw_.find("\n") + 1;
	std::string::size_type	head_len = this->raw_.find("\r\n\r\n") - head_start;

	this->header_ = this->raw_.substr(head_start, head_len);
	// std::cout << "RAW HEADER:\n" << this->header_ << std::endl;

	MapIter previous;
	InsertReturn check;
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
		if (!check.second)
			return false;
		previous = check.first;
	}
	// std::cout << "HEADERS:\n";
	// for (MapIter it = this->headers_.begin(); \
	// 	it != this->headers_.end(); ++it) {
	// 	std::cout << it->first << ":" << it->second << std::endl;
	// }
	// std::cout << "======================" << std::endl;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// --- GETTERS ---

// TODO: expand this check, now is valid only for methods without payload
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
