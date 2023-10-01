#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	SetupAddrInfo();
	SetupSocket();
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

// get sockaddr, IPv4 or IPv6:
void	*Server::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

//Apparently addrinfo is a list, so we have to loop through it and bind the socket to the first we can
//? still unsure why it's a list ):
void	Server::SetupSocket()
{
	for(struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		//socket() gives it an fd
		if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1)
		{
			std::cerr << "server: socket" << std::endl;
			continue;
		}
	
		//binding it gives it a port
		if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{
			std::cerr << "bind error" << std::endl;
			continue;
		}
		break;
	}
}

void	Server::Listen()
{
	//the backlog is how many connections are possible to be queued for further acceptance
	//otherwise if it overflows I think it gives out an error
	if (listen(sock, BACKLOG) == -1)
		std::cerr << "listening error" << std::endl;
	std::cout << "Listening on port fd " << sock << std::endl;
}

void	Server::Accept()
{
	int							newfd;
	char						s[INET6_ADDRSTRLEN];
	socklen_t					addr_size;
	struct	sockaddr_storage	their_addr;

	addr_size = sizeof their_addr;
	newfd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);
	if (newfd == -1)
		std::cerr << "accepting error" << std::endl;
	else if (newfd == 0)
		std::cerr << "connection closed from remote" << std::endl;
	//!inet_ntop is not allowed, remove later
	inet_ntop(their_addr.ss_family,
	get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
	std::cout << "server: got connection from " << s << std::endl;
	if (!fork())
	{
		close(sock);
		if (send(newfd, "Hello world!", 12, 0) == -1)
			std::cerr << "send error" << std::endl;
		close(newfd);
		exit(0);
	}
	close(newfd);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */