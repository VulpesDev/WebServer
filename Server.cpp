#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	SetupAddrInfo();
	SetupSocket();
	BindSocket();
	Listen();
	Accept();
}

Server::Server( const Server & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
	freeaddrinfo(servinfo);
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs )
	{
		
	}
	return *this;
}

// std::ostream &			operator<<( std::ostream & o, Server const & i )
// {
// 	o << "Value = " << i.getValue();
// 	return o;
// }


/*
** --------------------------------- METHODS ----------------------------------
*/

//So this addrinfo struct holds the all the info for the server we need
//here is where we fill it with values and options
void	Server::SetupAddrInfo()
{
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0)
	{
	    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
	    exit(1);
	}
}

void	Server::SetupSocket()
{
	sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sock == -1)
	{
		std::cerr << "socket error" << std::endl;
	}
}

//So while the setup socket just gives socket an fd,
// binding it, assigns it a port to use
void	Server::BindSocket()
{
	if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		std::cerr << "bind error" << std::endl;

}

void	Server::Listen()
{
	//the backlog is how many connections are possible to be queued for further acceptance
	//otherwise if it overflows I think it gives out an error
	if (listen(sock, BACKLOG) == -1);
		std::cerr << "listening error" << std::endl;
	std::cout << "Listening on port fd " << sock << std::endl;
}

void	Server::Accept()
{
	int							newfd;
	socklen_t					addr_size;
	struct	sockaddr_storage	their_addr;

	addr_size = sizeof their_addr;
	newfd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);
	if (newfd == -1)
		std::cerr << "accepting error" << std::endl;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */