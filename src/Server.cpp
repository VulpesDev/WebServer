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
	int const	max_events = 32;
	epoll_event	events[max_events];
	int const	timeout = 4200;

	std::signal(SIGINT, signal_handler);
	std::cout << "Listening for connections..." << std::endl;
	while (true) {
		int n_events = epoll_wait(this->epoll_fd_, events, max_events, timeout);
		if (n_events == -1) {
			std::cerr << "Failed epoll_wait" << std::endl;
			return ;
		}
		for (int i = 0; i < n_events; ++i) {
			int fd = events[i].data.fd;
			if (this->listen_fds_.find(fd) != this->listen_fds_.end()) {
				this->add_client(fd);
				continue;
			}
			if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
				this->close_connection(fd);
				continue;
			}
			if (events[i].events & EPOLLIN) {
				this->handle_request(fd);
			}
			if (events[i].events & EPOLLOUT) {
				std::cout << "Ready to send dummy reply" << std::endl;
				this->send_dummy_reply(fd);
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
		int re = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re) == -1) {
			std::cerr << "Failed to set socket as reusable" << std::endl;
			return false;
		}
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
	epoll_event			event = {};

	int client_fd = accept(listen_fd, NULL, NULL);
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
		std::cout << "Client connected" << std::endl;
		typedef std::map<int, std::string>::iterator mapIterator;
		mapIterator it = this->inbound_.lower_bound(client_fd);
		if (it != this->inbound_.end() && \
			this->inbound_.key_comp()(client_fd, it->first)) {
				it->second.clear();
		} else {
			this->inbound_.insert(it, std::make_pair(client_fd, ""));
		}
	}
}

void Server::close_connection(int fd)
{
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1) {
		std::cerr << "Failed to remove fd from epoll" << std::endl;
	}
	(void)close(fd);
	this->inbound_.erase(fd);
	std::cout << "Closed connection with client" << std::endl;
}

void Server::handle_request(int fd)
{
	static int const	len = 8192;
	char				buff[len];

	switch (recv(fd, buff, len, MSG_DONTWAIT)) {
		case -1:
			return ;
		case 0:
			this->close_connection(fd);
			return ;
		default:
			// TODO: process received request
			std::cout << "REQUEST:\r\n" << buff << std::endl;
	}

	typedef std::map<int, std::string>::iterator mapIterator;
	mapIterator it = this->inbound_.find(fd);
	if (it == this->inbound_.end()) {
		this->inbound_.insert(it, std::make_pair(fd, buff));
	} else {
		it->second.append(buff);
	}
	
	// TODO: proceed only when full request received
	epoll_event event = {};
	event.events = EPOLLOUT;
	event.data.fd = fd;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_MOD, fd, &event)) {
		std::cerr << "Failed to add fd to epoll_ctl" << std::endl;
		this->close_connection(fd);
	}
}

void Server::send_dummy_reply(int fd)
{
	std::string reply =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 0\r\n"
		"Keep-Alive: timeout=1, max=1\r\n"
		"Accept-Ranges: bytes\r\n"
		"Connection: close\r\n\r\n";

	ssize_t s = send(fd, reply.c_str(), reply.size(), 0);
	if (!s || s == -1) {
		std::cout << "Send failed" << std::endl;
		this->close_connection(fd);
		return ;
	}
	std::cout << "Dummy reply sent, size: " << s << std::endl;
	this->close_connection(fd);
}
