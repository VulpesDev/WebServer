/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 23:03:11 by mcutura           #+#    #+#             */
/*   Updated: 2024/02/04 23:08:28 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HTTPMESSAGE_HPP
# define WEBSERV_HTTPMESSAGE_HPP

#include <map>
#include <string>
#include <sstream> 
#include <iostream>
#include <unordered_map>

#define MAX_CHUNK_SIZE 8192

struct HTTPRequest {
    std::string method;
    std::string path;
    std::string http_version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HTTPRequestParser {
public:
    HTTPRequestParser(char *raw_request, size_t len);
	HTTPRequest parse();

private:
    std::string raw_request;
};

class HTTPResponse {
public:
    HTTPResponse(int status_code);

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
