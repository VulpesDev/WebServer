/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 02:04:45 by mcutura           #+#    #+#             */
/*   Updated: 2024/01/25 18:30:05 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <csignal>

// #include <Server.hpp>

// int	main(void)
// {
// 	Server webserv;

// 	if (!webserv.initialize())
// 		return 1;
// 	std::signal(SIGINT, Server::stop);
// 	webserv.start();
// 	return 0;
// }

#include <HttpMessage.hpp>

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
	
	std::cout << std::endl << std::endl << std::endl;
	
    HTTPResponse response(200, get_status_message(200));
    response.setHeader("Date", "Mon, 25 Jan 2024 12:00:00 GMT");
    response.setHeader("Server", "Apache/2.4.41 (Unix)");
    response.setHeader("Content-Type", "text/html");
    response.setBody("<html><body>OK</body></html>");

    std::string raw_response = response.getRawResponse();
    std::cout << raw_response << std::endl;


	

    return 0;
}
