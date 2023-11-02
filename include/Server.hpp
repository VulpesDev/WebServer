/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 02:41:52 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/25 02:41:52 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVER_HPP
# define WEBSERV_SERVER_HPP

# include <cstdlib>
# include <fstream>
# include <iostream>
# include <map>
# include <set>
# include <sstream>
# include <string>

# include <arpa/inet.h>
# include <fcntl.h>
# include <netdb.h>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>

# include <ServerConfig.hpp>
# include <HttpMessage.hpp>

extern bool	stop_server;

class Server
{
	public:
		Server();
		~Server();

		/**
		 * @brief sets and binds sockets for the server on given/default port(s)
		 * 
		 * @return boolean value reporting success of operations
		 */
		bool initialize();
		/**
		 * @brief starts polling loop on listening socket(s) and any connected
		 * client fds, receives requests and sends replies
		 * 
		 */
		void start();
		/**
		 * @brief stops the server.
		 * A static function that can also be used as valid signal handler
		 *
		 */
		static void stop(int);

	private:
		static int const			BACKLOG_ = 10;
		int							epoll_fd_;
		std::string					name_;
		std::set<std::string>		port_;
		std::set<int>				listen_fds_;
		std::set<int>				connections_;
		std::map<int, Request>		inbound_;
		std::map<int, std::string>	outbound_;

		void cleanup();
		bool setup_socket(std::string const &port);
		void add_client(int listen_fd);
		void close_connection(int fd);
		void handle_request(int fd);
		void queue_reply(int fd, Reply const &reply);
		bool send_reply(int fd, std::string &reply);

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif  /* WEBSERV_SERVER_HPP */
