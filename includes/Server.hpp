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

# include <csignal>
# include <cstdlib>
# include <iostream>
# include <iterator>
# include <set>
# include <string>

# include <arpa/inet.h>
# include <netdb.h>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>

class Server
{
	public:
		Server();
		~Server();

		/* sets and binds sockets for the server on given/default port(s) */
		bool initialize();
		/* starts polling loop on listening socket(s) and any connected fds */
		void start();

	private:
		static int const		BACKLOG_ = 10;
		bool					isrunning_;
		int						epoll_fd_;
		std::set<std::string>	port_;
		std::set<int>			listen_fds_;

		static void signal_handler(int signo);
		bool setup_socket(std::string const &port);
		void add_client(int listen_fd);
		void close_connection(int fd);
		void handle_request(int fd);

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif  /* WEBSERV_SERVER_HPP */
