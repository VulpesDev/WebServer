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
	addrinfo *servinfo;
	addrinfo *ptr;
	addrinfo hints = {};

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, MYPORT, &hints, &servinfo);
	if (status) {
		std::cerr << "getaddrinfo error: " \
			<< gai_strerror(status) << std::endl;
		return false;
	}

	if ((this->epoll_fd_ = epoll_create(42)) == -1) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return false;
	}

	for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
		this->listen_fd_ = socket(ptr->ai_family, \
			ptr->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, ptr->ai_protocol);
		if (this->listen_fd_ == -1)
			continue;
		if (bind(this->listen_fd_, ptr->ai_addr, ptr->ai_addrlen) == -1)
			continue;
		break;
	}
	if (!ptr) {
		std::cerr << "Failed to initialize server socket" << std::endl;
		freeaddrinfo(servinfo);
		return false;
	}
	freeaddrinfo(servinfo);
	if (listen(this->listen_fd_, BACKLOG_) == -1) {
		std::cerr << "Failed to listen on bound socket" << std::endl;
	}

	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = this->listen_fd_;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, this->listen_fd_, &event)) {
		std::cerr << "Failed to add socket_fd to epoll_ctl" << std::endl;
		return false;
	}
	std::cout << "Server initialized" << std::endl;

	return true;
}

void Server::start()
{
	int const max_events = 16;
	epoll_event events[max_events];
	int const timeout = 4200;
	sockaddr_storage addr;
	socklen_t addr_size = sizeof addr;
	epoll_event event = {};

	std::cout << "Listening for connections..." << std::endl;
	while (true) {
		int event_count = epoll_wait(this->epoll_fd_, events, max_events, timeout);
		if (event_count == -1) {
			std::cerr << "Failed epoll_wait" << std::endl;
			return ;
		}
		for (int i = 0; i < event_count; ++i) {
			if (events[i].data.fd == this->listen_fd_) {
				// new client connection
				int client_fd = accept(this->listen_fd_, (sockaddr *)&addr, &addr_size);
				if (client_fd == -1) {
					std::cerr << "Failed to accept connection" << std::endl;
					continue;
				}
				memset(&event, 0, sizeof event);
				event.events = EPOLLIN;
				event.data.fd = client_fd;
				if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, client_fd, &event)) {
					std::cerr << "Failed to add client_fd to epoll_ctl" << std::endl;
					continue;
				}
			} else if (events[i].events & (EPOLLERR | EPOLLHUP)) {
				// close connection
				if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1) {
					std::cerr << "Failed to remove fd from epoll" << std::endl;
					continue;
				}
				(void)close(events[i].data.fd);
			} else {
				// handle client request
			}
		}
	}
}
