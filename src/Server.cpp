/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 02:08:32 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/25 02:08:32 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{}

Server::~Server()
{}

bool Server::initialize()
{
	struct addrinfo *servinfo;
	struct addrinfo *ptr;
	struct addrinfo hints = {};

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, MYPORT, &hints, &servinfo);
	if (status) {
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		return false;
	}

	for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
		this->listen_sd_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (this->listen_sd_ == -1)
			continue;
		if (bind(this->listen_sd_, ptr->ai_addr, ptr->ai_addrlen) == -1)
			continue;
		break;
	}
	if (!ptr) {
		std::cerr << "Failed to initialize server" << std::endl;
		return false;
	} else {
		std::cout << "Server initialized, listening on socket " << this->listen_sd_ << std::endl;
	}
	freeaddrinfo(servinfo);

	//Set the socket to nonblocking (so it can return immediately if there is no event -1 I think)
	// if (fcntl(listen_sd, F_SETFL, O_NONBLOCK) == -1)
	// 	std::cerr << "fcntl() failed" << std::endl;

	return true;
}

void Server::start()
{
	std::cout << "Server listening for connections..." << std::endl;
}
