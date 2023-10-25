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

# include <cstring>
# include <iostream>

# include <netdb.h>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>

# define MYPORT "14242" // TODO - remove after implementing init from config

class Server
{

	public:
		Server();
		~Server();

		bool initialize();
		void start();

	private:
		int					listen_fd_;
		int					epoll_fd_;
		static const int	BACKLOG_ = 10;

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif  /* SERVER_H */
