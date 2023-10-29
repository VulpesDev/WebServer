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

#include <Server.hpp>

////////////////////////////////////////////////////////////////////////////////
// --- SIGNAL HANDLING SUPPORT ---

bool	stop_server(false);

void Server::stop(int)
{
	stop_server = true;
}

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---

Server::Server()
{
	this->epoll_fd_ = -1;
	this->name_ = "Sp1d3rServ";
	if (!this->port_.insert("8080").second)
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

////////////////////////////////////////////////////////////////////////////////
// --- METHODS ---

bool Server::initialize()
{
	if (this->port_.empty())
		return false;
	for (std::set<std::string>::iterator it = this->port_.begin(); \
		it != this->port_.end(); ++it) {
			if (!this->setup_socket(*it)) {
				// TODO cleanup crew to aisle 42
				return false;
			}
		}
	// epoll_create(int) requires int param greater than 0 that is IGNORED
	// epoll_create1(int flags) would allow passing EPOLL_CLOEXEC instead
	// but it's not on our list of allowed functions for this project, so:
	if ((this->epoll_fd_ = epoll_create(42)) == -1) {
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		// TODO cleanup here
		return false;
	}
	fcntl(this->epoll_fd_, F_SETFL, O_CLOEXEC);

	epoll_event event = {};
	for (std::set<int>::iterator it = this->listen_fds_.begin(); \
		it != this->listen_fds_.end(); ++it) {
			event.events = EPOLLIN;
			event.data.fd = *it;
			if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, *it, &event)) {
				std::cerr << "Failed to add fd to epoll_ctl" << std::endl;
				// TODO cleanup here also
				return false;
			}
		}
	std::cout << "Server initialized" << std::endl;
	return true;
}

void Server::start()
{
	int const	timeout = 4200;
	int const	max_events = 32;
	epoll_event	events[max_events];

	std::cout << "Listening for connections..." << std::endl;
	while (!stop_server) {
		int n_events = epoll_wait(this->epoll_fd_, events, max_events, timeout);
		if (n_events == -1 && !stop_server) {
			std::cerr << "Failed epoll_wait" << std::endl;
			// TODO call cleanup crew here pls
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
				// handle sending replies
				typedef std::map<int, std::string>::iterator mapIterator;
				mapIterator it = this->outbound_.find(fd);
				if (it != this->outbound_.end()) {
					this->send_reply(fd, it->second);
					this->outbound_.erase(fd);
					// TODO: only close if HTTP/1.0 or Connection: Close
					this->close_connection(fd);
				}
			}
		}
	}
	std::cout << " Server shutting down" << std::endl;
	// TODO add proper cleanup here, it should not rely on calling destructor.
	// That way the Server object can be restarted
	// without restarting the whole program
}

////////////////////////////////////////////////////////////////////////////////
// --- INTERNALS ---

bool Server::setup_socket(std::string const &port)
{
	int			sfd(-1);
	addrinfo	*servinfo;
	addrinfo	*ptr;
	addrinfo	hints = {};

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
		if (sfd == -1)
			continue;
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			(void)close(sfd);
			continue;
		}
		int re = 1L;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re) == -1) {
			std::cerr << "Failed to set socket as reusable" << std::endl;
			(void)close(sfd);
			return false;
		}
		break;
	}
	// TODO: investigate why it still fails here sometimes when re-run
	if (!ptr) {
		std::cerr << "Failed to initialize server socket" << std::endl;
		freeaddrinfo(servinfo);
		return false;
	}
	freeaddrinfo(servinfo);
	if (listen(sfd, Server::BACKLOG_) == -1) {
		std::cerr << "Failed to listen on bound socket" << std::endl;
		(void)close(sfd);
		return false;
	}
	if (!this->listen_fds_.insert(sfd).second) {
		std::cerr << "Failed to store listening socket" << std::endl;
		(void)close(sfd);
		return false;
	}
	return true;
}

//*
void Server::add_client(int listen_fd)
{
	// We default to be anonymous accepting (non-tracking) server
	int client_fd = accept(listen_fd, NULL, NULL);
	if (client_fd == -1) {
		std::cerr << "Failed to accept connection" << std::endl;
		return ;
	}
	// with accept4(..., SOCK_NONBLOCK | SOCK_CLOEXEC) instead of accept(...)
	// we would be able to skip this syscall
	// alas it's not on the list of allowed C functions for this project, so:
	fcntl(client_fd, F_SETFL, O_NONBLOCK | O_CLOEXEC);

	epoll_event event = {};
	event.events = EPOLLIN;  // poll listen sockets only for read/recv
	event.data.fd = client_fd;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_ADD, client_fd, &event)) {
		std::cerr << "Failed to add client_fd to epoll_ctl" << std::endl;
		(void)close(client_fd);
	} else if (!this->connections_.insert(client_fd).second) {
		std::cerr << "Failed storing client fd: " << client_fd << std::endl;
		this->close_connection(client_fd);
	} else {
		std::cout << "Client connection: " << client_fd << std::endl;
	}
}
/**/

void Server::close_connection(int fd)
{
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1) {
		std::cerr << "Failed to remove fd from epoll" << std::endl;
	}
	(void)close(fd);
	this->connections_.erase(fd);
	this->inbound_.erase(fd);
	std::cout << "Closed connection with client: " << fd << std::endl;
}

void Server::handle_request(int fd)
{
	static int const	len = 4096;
	char				buff[len];

	switch (ssize_t r = recv(fd, buff, len - 1, MSG_DONTWAIT)) {
		case -1:
			return ;
		case 0:
			std::cout << "Client " << fd \
				<< " has closed connection" << std::endl;
			this->close_connection(fd);
			return ;
		default:
			buff[r] = '\0';
	}

	Request req;
	typedef std::map<int, Request>::iterator MapIterator;
	MapIterator mapit = this->inbound_.find(fd);
	if (mapit == this->inbound_.end()) {
		req = Request(buff);
	} else {
		req = mapit->second;
		req.append(buff);
		this->inbound_.erase(mapit);
	}
	if (int status = req.validate_start_line()) {
		this->send_reply(fd, Reply(req.version(), status).get());
		std::cout << "Status: " << status << std::endl;
		this->close_connection(fd);
	}
	if (!req.is_complete()) {
		this->inbound_[fd] = req;
		return ;
	}

	epoll_event event = {};
	event.events = EPOLLOUT;
	event.data.fd = fd;
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_MOD, fd, &event)) {
		std::cerr << "Failed to add fd to epoll_ctl" << std::endl;
		this->close_connection(fd);
	}

	// TODO: really parse request before generating reply
	Reply rep(req.version(), 200L);
	typedef std::map<int, std::string>::iterator ReplyIterator;
	ReplyIterator replit = this->outbound_.find(fd);
	if (replit == this->outbound_.end()) {
		this->outbound_.insert(replit, std::make_pair(fd, rep.get()));
	} else {
		replit->second.append(rep.get());
	}
}

// TODO: check size sent and send remainder of reply back to pool
// or something like that
void Server::send_reply(int fd, std::string const &reply)
{
	ssize_t s = send(fd, reply.c_str(), reply.size(), 0);
	if (!s || s == -1) {
		std::cout << "Send failed" << std::endl;
		this->close_connection(fd);
		return ;
	}
	std::cout << "Reply sent, size: " << s << std::endl;
}
