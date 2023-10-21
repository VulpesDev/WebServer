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
	HttpRequest	req("GET	 	  /background.png		  HTTP/1.1 		 \nHost: localhost:80000\n");
	return (0);
}