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
	HttpRequest	req(httpRequest);
	return (0);
}