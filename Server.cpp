#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	SetupAddrInfo();
	SetupListenSocket();
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

//Apparently addrinfo is a list, so we have to loop through it and bind the socket to the first element where we can
//? still unsure why it's a list ):
void	Server::SetupListenSocket()
{
	for(struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		//socket() gives it an fd
		if ((listen_sd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1)
		{
			std::cerr << "server: socket" << std::endl;
			continue;
		}
	
		//binding it gives it a port (using some namespace for address family in the /dev folder (some serious system stuff))
		if (bind(listen_sd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{
			std::cerr << "bind error" << std::endl;
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	
	//Set the socket to nonblocking (so it can return immediately if there is no event -1 I think)
	if (fcntl(listen_sd, F_SETFL, O_NONBLOCK) == -1)
		std::cerr << "fcntl() failed" << std::endl;
}

void	Server::Listen()
{
	//the backlog is how many connections are possible to be queued for further acceptance
	//otherwise if it overflows I think it gives out an error
	if (listen(listen_sd, BACKLOG) == -1)
		std::cerr << "listening error" << std::endl;
	std::cout << "Listening on port fd " << listen_sd << std::endl;
}

void	Server::Accept()
{
	int	max_sd, new_sd;
	int	desc_ready, end_server = FALSE, close_conn;
	int		rec;
	char	recvBuff[8000]; ///? According to chatGPT an  HTTP server handling requests and responses might usually be 4-16 kylobytes
	//there are two fd sets because select() modifies the set thats passed in, so we pass in working_set which is a copy of the actual set
	fd_set	master_set, working_set;

	//Initialize the master fd_set
	FD_ZERO(&master_set);
	max_sd = listen_sd;
	FD_SET(listen_sd, &master_set);

	//Loop for incoming connections on the listen sock
	//or for any of the already connected sockets
	do
	{
		memcpy(&working_set, &master_set, sizeof(master_set));
		desc_ready = select(max_sd + 1, &working_set, NULL, NULL, NULL);
		if (desc_ready < 0)
		{
			std::cerr << "select() failed" << std::endl;
			break;
		}
		if (desc_ready == 0)
		{
			std::cerr << "select() timed out" << std::endl;
			break;
		}
		//select found desc that are active, so loop through them
		for (int i = 0; i <= max_sd && desc_ready > 0; ++i)
		{
			if (FD_ISSET(i, &working_set))
			{
				std::cout << "found a set socket!" << std::endl;
				desc_ready -= 1;
				//if its the listen sd then add the new connection to the masterset
				if (i == listen_sd)
				{
					std::cout << "Reading from listening socket" << std::endl;
					do
					{
						new_sd = accept(listen_sd, NULL, NULL);
						if (new_sd < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								std::cerr << "accept() failed" << std::endl;
								end_server = TRUE;
							}
							std::cout << "Breaking out of loop" << std::endl;
							break;
						}
						std::cout << "New incoming connection - " << new_sd << std::endl;
						FD_SET(new_sd, &master_set);
						if (new_sd > max_sd)
							max_sd = new_sd;
						std::cout << "Max sd - " << max_sd << std::endl;
					} while (new_sd != -1);
				}
				//otherwise some of the other sd-s are responding so handle them
				else
				{
					std::cout << "Descriptor is readable - " << i << std::endl;
					close_conn = FALSE;
					do
					{
						rec = recv(i, recvBuff, sizeof(recvBuff), 0);
						if (rec < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								std::cerr << "recv() failed with " << strerror(errno) << std::endl;
								close_conn = TRUE;
							}
							break;
						}
						if (rec == 0)
						{
							std::cout << "Client disconnected" << std::endl;
							close_conn = TRUE;
							break;
						}
						std::cout << "Bytes received - " << rec << std::endl;
						rec = send(i, recvBuff, rec, 0);
						if (rec < 0)
						{
							std::cerr << "send() failed" <<  std::endl;
							close_conn = TRUE;
							break;
						}
					} while (TRUE);
					//if the connection is closed then close the sd-s and clear the element from the master set
					if (close_conn)
					{
						close(i);
						FD_CLR(i, &master_set);
						if (i == max_sd)
						{
							while (FD_ISSET(max_sd, &master_set) == FALSE)
								max_sd--;
						}
					}
				}
			}
		}
		
	} while (end_server == FALSE);
	
	//clean up all of the sockets that are open
	for (int i = 0; i < max_sd; i++)
	{
		if (FD_ISSET(i, &master_set))
			close(i);
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */