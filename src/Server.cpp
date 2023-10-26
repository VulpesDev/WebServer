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
{
	if (!port_.insert("14242").second)
		std::cerr << "Failed to store default listening port" << std::endl;
}

Server::~Server()
{
	for (std::set<int>::iterator it = this->listen_fds_.begin(); \
		it != this->listen_fds_.end(); ++it) {
			(void)close(*it);
		}
	(void)close(this->epoll_fd_);
}

bool Server::initialize()
{
	if (this->port_.empty())
		return false;
	for (std::set<std::string>::iterator it = this->port_.begin(); \
		it != this->port_.end(); ++it) {
			if (!this->setup_socket(*it)) {
				return false;
			}
		}
	if ((this->epoll_fd_ = epoll_create(42)) == -1) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		return false;
	}
	epoll_event event = {};
	for (std::set<int>::iterator it = this->listen_fds_.begin(); \
		it != this->listen_fds_.end(); ++it) {
			event.events = EPOLLIN;
			event.data.fd = *it;
			if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, *it, &event)) {
				std::cerr << "Failed to add fd to epoll_ctl" << std::endl;
				return false;
			}
		}
	std::cout << "Server initialized" << std::endl;
	return true;
}

void Server::start()
{
	int const	max_events = 16;
	epoll_event	events[max_events];
	int const	timeout = 4200;

	this->isrunning_ = true;
	std::signal(SIGINT, signal_handler);
	std::cout << "Listening for connections..." << std::endl;
	while (this->isrunning_) {
		int n_events = epoll_wait(this->epoll_fd_, events, max_events, timeout);
		if (n_events == -1) {
			std::cerr << "Failed epoll_wait" << std::endl;
			return ;
		}
		for (int i = 0; i < n_events; ++i) {
			int fd = events[i].data.fd;
			if (this->listen_fds_.find(fd) != this->listen_fds_.end()) {
				this->add_client(fd);
			} else if (events[i].events & (EPOLLERR | EPOLLHUP)) {
				this->close_connection(fd);
			} else {
				this->handle_request(fd);
			}
		}
	}
}

void Server::signal_handler(int signo)
{
	std::cout << " Server shutting down" << std::endl;
	std::exit(signo);
}

bool Server::setup_socket(std::string const &port)
{
	addrinfo	*servinfo;
	addrinfo	*ptr;
	addrinfo	hints = {};
	int			sfd(-1);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (int ret = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) {
		std::cerr << "getaddrinfo error: " \
			<< gai_strerror(ret) << std::endl;
		return false;
	}
	for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) {
		sfd = socket(ptr->ai_family, \
			ptr->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC, ptr->ai_protocol);
		if (sfd == -1 || bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == -1)
			continue;
		break;
	}
	if (!ptr) {
		std::cerr << "Failed to initialize server socket" << std::endl;
		freeaddrinfo(servinfo);
		return false;
	}
	freeaddrinfo(servinfo);
	if (listen(sfd, this->BACKLOG_) == -1) {
		std::cerr << "Failed to listen on bound socket" << std::endl;
		(void)close(sfd);
		return false;
	}
	if (!this->listen_fds_.insert(sfd).second) {
		std::cerr << "Failed to store listening socket" << std::endl;
		return false;
	}
	return true;
}

void Server::add_client(int listen_fd)
{
	sockaddr_storage	addr;
	socklen_t			addr_size = sizeof addr;
	epoll_event			event = {};
	char 				ipstr[INET6_ADDRSTRLEN];
	int					port;

	int client_fd = accept(listen_fd, (sockaddr *)&addr, &addr_size);
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
		} else {
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
		case 0:
			this->close_connection(fd);
			break;
		case -1:
			std::cerr << "Failed to receive client request" << std::endl;
			break;
		default:
			// TODO: process received request
			std::cout << buff << std::endl;
	}
}
