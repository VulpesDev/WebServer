/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 23:03:11 by mcutura           #+#    #+#             */
/*   Updated: 2024/02/01 18:36:27 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HTTPMESSAGE_HPP
# define WEBSERV_HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <sstream> 
#include <iostream>
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
	HTTPRequest parse();

private:
    std::string raw_request;
};

class HTTPResponse {
public:
    HTTPResponse(int status_code, const std::string& reason_phrase)
        : status_code(status_code), reason_phrase(reason_phrase) {}

    void setHeader(const std::string& key, const std::string& value);

    void setBody(const std::string& body_content);

    std::string getRawResponse() const;

private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
};

std::string const get_status_message(int status);
std::string const generate_error_page(int status);
std::string const get_status_message_detail(int status);

////////////////////////////////////////////////////////////////////////////////
// --- METHODS ---

// Source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
std::string const get_status_message(int status);

////////////////////////////////////////////////////////////////////////////////
// --- INTERNALS ---

std::string const generate_error_page(int status);

#endif  // WEBSERV_HTTPMESSAGE_HPP
