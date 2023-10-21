/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/01 14:24:56 by tvasilev          #+#    #+#             */
/*   Updated: 2023/10/01 14:57:54 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<iostream>
#include"Server.hpp"
#include"HttpRequest.hpp"

int	main(void)
{
	// Server	serv;
	//std::cout << "Hello World!" << std::endl;
	std::string httpRequest = "GET /index.html HTTP/1.1\n"
                       "Host: www.example.com\n"
                       "User-Agent: MyHttpClient/1.0\n"
                       "Accept-Language: en-US\n"
                       "Connection: close\n";
	std::string request1 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n";
	std::string request2 = "GET /index.html HTTP/1.1\r\n";
	std::string request3 = "POST /submit-form HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/json\r\nContent-Length: 30\r\n\r\n{\"data\": \"some payload data\"}";
	std::string request4 = "GET /search?query=example&lang=en HTTP/1.1\r\nHost: www.example.com\r\n";
	std::string request5 = "GET /index.html HTTP/1.2\r\nHost: www.example.com\r\n";
	std::string request6 = "GET  /index.html  HTTP/1.1\r\nHost:   www.example.com\r\n";
	std::string request7 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\n";
	std::string request8 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nX-Custom-Header: SomeValue\r\n";
	std::string request9 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nIf-Modified-Since: Thu, 01 Jan 1970 00:00:00 GMT\r\n";
	std::string request10 = "GET /index.html HTTP/1.1\r\nHost:\r\n";

	std::string request_new1 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nAccept: text/html\r\nConnection: close\r\n";
	std::string request_new2 = "POST /submit-data HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nContent-Type: application/json\r\nContent-Length: 42\r\nConnection: keep-alive\r\n\r\n{\"name\": \"John\", \"age\": 30}\r\n";
	std::string request_new3 = "GET /resource HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nIf-Modified-Since: Thu, 01 Jan 1970 00:00:00 GMT\r\nConnection: close\r\n";
	std::string request_new4 = "GET /profile HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nCookie: session_id=abc123\r\nCookie: user_prefs=dark-theme\r\nConnection: close\r\n";
	std::string request_new5 = "GET /index.html HTTP/1.1\r\nHost:     (Malformed Host Header)\r\nUser-Agent: My HTTP Client\r\nConnection: close\r\n";
	std::string request_new6 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nX-Long-Header: (Very long string repeated many times...)\r\nConnection: close\r\n";
	std::string request_new7 = "GET /data HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nX-Special-Header: Value with @ and # characters!\r\nConnection: close\r\n";
	std::string request_new8 = "GET /protected-resource HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nAuthorization: Bearer abcdefgh123456789\r\nConnection: close\r\n";
	std::string request_new9 = "GET /compressed-data HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: My HTTP Client\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n";
	std::string request_new10 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n";


	HttpRequest	req(request_new1);
	std::cout << std::endl;
	std::cout << std::endl;
	HttpRequest	req1(request_new2);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	req2(request_new3);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	req3(request_new4);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	req4(request_new5);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	rea5(request_new6);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	wreq(request_new7);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	rdeq(request_new8);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	rezq(request_new9);
	std::cout << std::endl;
	std::cout << std::endl;

	HttpRequest	recq(request_new10);
	return (0);
}