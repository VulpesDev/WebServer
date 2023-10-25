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

Server::Server() : port_("14242")
{}

Server::~Server()
{}

bool Server::initialize()
{
	if (!this->setup_socket())
		return false;
	if (listen(this->listen_fd_, this->BACKLOG_) == -1) {
		std::cerr << "Failed to listen on bound socket" << std::endl;
		(void)close(this->listen_fd_);
		return false;
	}
	if ((this->epoll_fd_ = epoll_create(42)) == -1) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		(void)close(this->listen_fd_);
		return false;
	}

	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = this->listen_fd_;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, this->listen_fd_, &event)) {
		std::cerr << "Failed to add socket_fd to epoll_ctl" << std::endl;
		(void)close(this->listen_fd_);
		(void)close(this->epoll_fd_);
		return false;
	}
	std::cout << "Server initialized" << std::endl;
	return true;
}

void Server::start()
{
	int const	max_events = 16;
	epoll_event	events[max_events];
	int const	timeout = 4200;

	std::cout << "Listening for connections..." << std::endl;
	while (true) {
		int n_events = epoll_wait(this->epoll_fd_, events, max_events, timeout);
		if (n_events == -1) {
			std::cerr << "Failed epoll_wait" << std::endl;
			return ;
		}
		for (int i = 0; i < n_events; ++i) {
			if (events[i].data.fd == this->listen_fd_) {
				this->add_client();
			} else if (events[i].events & (EPOLLERR | EPOLLHUP)) {
				this->close_connection(events[i].data.fd);
			} else {
				this->handle_request(events[i].data.fd);
			}
		}
	}
}

bool Server::setup_socket()
{
	addrinfo	*servinfo;
	addrinfo	*ptr;
	addrinfo	hints = {};

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (int status = getaddrinfo(NULL, this->port_.c_str(), &hints, &servinfo)) {
		std::cerr << "getaddrinfo error: " \
			<< gai_strerror(status) << std::endl;
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
	return true;
}

void Server::add_client()
{
	sockaddr_storage	addr;
	socklen_t			addr_size = sizeof addr;
	epoll_event			event = {};
	char 				ipstr[INET6_ADDRSTRLEN];
	int					port;

	int client_fd = accept(this->listen_fd_, (sockaddr *)&addr, &addr_size);
	if (client_fd == -1) {
		std::cerr << "Failed to accept connection" << std::endl;
		return ;
	}
	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, client_fd, &event)) {
		std::cerr << "Failed to add client_fd to epoll_ctl" << std::endl;
		(void)close(client_fd);
	} else {
		if (addr.ss_family == AF_INET) {
			sockaddr_in *s = (struct sockaddr_in *)&addr;
			port = ntohs(s->sin_port);
			inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		} else { // AF_INET6
			struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
			port = ntohs(s->sin6_port);
			inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		}
		std::cout << "Connected client " << ipstr << ":" << port << std::endl;
	}
}

void Server::close_connection(int fd)
{
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1) {
		std::cerr << "Failed to remove fd from epoll" << std::endl;
	}
	(void)close(fd);
	std::cout << "Closed connection with client" << std::endl;
}

void Server::handle_request(int fd)
{
	static int const	len = 4096;
	char				buff[len];

	switch (recv(fd, buff, len, 0)) {
		case -1:
			std::cerr << "Failed to receive client request" << std::endl;
			break;
		case 0:
			this->close_connection(fd);
			break;
		default:
			// TODO: process received request
			std::cout << buff << std::endl;
	}
}
