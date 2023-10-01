#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <cstdlib>
# include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
# define MYPORT "3490"
# define BACKLOG 10

class Server
{

	public:

		Server();
		Server( Server const & src );
		~Server();

		Server &		operator=( Server const & rhs );

	private:
		void	SetupAddrInfo();
		void	SetupSocket();
		void	Listen();
		void	Accept();

		void	*get_in_addr(struct sockaddr *sa);
		int				sock;
		struct addrinfo *servinfo;  // will point to the results
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************************** SERVER_H */