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
# include <iterator>
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

extern bool	stop_server;

class Server
{
	public:
		Server();
		~Server();

		/* sets and binds sockets for the server on given/default port(s) */
		bool initialize();
		/* starts polling loop on listening socket(s) and any connected fds */
		void start();
		/* stops the server loop 
		 * can be registered as valid signal handler
		 */
		static void stop(int);

	private:
		static int const			BACKLOG_ = 10;
		int							epoll_fd_;
		std::string					name_;
		std::set<std::string>		port_;
		std::set<int>				listen_fds_;
		std::map<int, std::string>	inbound_;

		bool setup_socket(std::string const &port);
		void add_client(int listen_fd);
		void close_connection(int fd);
		void handle_request(int fd);
		void get_payload(std::string const &path, std::string &payload);
		void generate_reply(std::string const &req, std::string &rep);
		void send_reply(int fd, std::string const &reply);
		void send_dummy_reply(int fd);

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif  /* WEBSERV_SERVER_HPP */
