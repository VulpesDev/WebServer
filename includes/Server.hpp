#ifndef SERVER_HPP
# define SERVER_HPP

# include <cstdlib>
# include <cstring>
# include <iostream>
# include <string>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <errno.h>
# include <fcntl.h>

# define FALSE 0
# define TRUE 1
# define MYPORT "3490"
# define BACKLOG 10

class Server
{

	public:
		Server();
		~Server();

		bool initialize();
		void start();

	private:
		int		listen_sd_;

		/* = delete */
		Server(Server const & src);
		Server &operator=(Server const & rhs);
};

#endif /* ********************************************************** SERVER_H */