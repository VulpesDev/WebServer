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

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
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

		bool initialize();
		void start();

	private:
		static int const	BACKLOG_ = 10;
		int					listen_fd_;
		int					epoll_fd_;
		std::string			port_;

		bool setup_socket(void);
		void add_client(void);
		void close_connection(int fd);
		void handle_request(int fd);

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif  /* SERVER_H */
