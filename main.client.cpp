/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/01 16:57:43 by tvasilev          #+#    #+#             */
/*   Updated: 2023/10/01 17:05:58 by tvasilev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <cstdlib>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include "sendall.hpp"
# include <errno.h>
# include <fcntl.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 8000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
    	std::cerr << "usage: client hostname" << std::endl;
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            std::cerr << "client: socket" << std::endl;
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cerr << "client: connect" << std::endl;
            continue;
        }

        break;
    }

    if (p == NULL) {
        std::cerr << "client: failed to connect" << std::endl;
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    std::cout << "client: connecting to " << s << std::endl;

    freeaddrinfo(servinfo); // all done with this structure

    char tosend[18850] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN kfc.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN bdf. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN kfa. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL puta. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN kart. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN more. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN lately. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN kill me. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN canadian gals. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN boobs. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN booze. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN mom. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN popcorn. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN cornpop. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN women. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN ummmm. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer eget justo nec lorem ullamcorper vulputate. Vivamus in ullamcorper nulla, nec consectetur dui. Fusce laoreet euismod metus, id euismod libero vestibulum sit amet. Quisque convallis dui at mi viverra, eu vestibulum neque semper. Nunc ac urna id arcu laoreet condimentum. Donec bibendum ex sed lorem scelerisque, a cursus libero lacinia. Nulla malesuada, dolor non vehicula mattis, justo velit scelerisque ante, eu feugiat tellus odio sit amet mi. Fusce cursus risus nec justo venenatis, sit amet fermentum neque posuere. Aliquam sit amet justo ac arcu laoreet facilisis. Vestibulum vel tincidunt tortor. Nunc mattis arcu id felis gravida, a suscipit massa pharetra. Vivamus vel tristique urna, vel interdum odio. Sed euismod auctor mi, ac finibus sapien posuere et. Curabitur lacinia, urna non scelerisque sagittis, odio erat luctus justo, eu cursus nisl arcu a mi. ESFKAFKLSKAFKLSAJFKLA ASJF KLASJKL JSKLAJ FKLJAKL JSFKLJKL ASKLFJ KLJSAKLFJKLA SKLFJKLAJ KLSJF KLAJSKL JFKLJASKLJ SAJ KLJSAKL JFKLJA KLJASKL JSKLAJF KLSJAKLF JAKLKKL THEN i need a back massage.";
    if ((numbytes = sendall(sockfd, tosend, 18850)) == -1)
    {
        std::cerr << "send" << std::endl;
        exit(1);
    }
    std::cout << "Sent bytes to server - " << numbytes << std::endl;
    std::cout << "client: received:" << std::endl;
    do
    {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
        {
            std::cerr << "recv" << std::endl;
            exit(1);
        }
        buf[numbytes] = '\0';
        if (numbytes == 0)
            break;
        std::cout << "bytes: " << numbytes << std::endl; 
        std::cout << buf << std::endl;
    } while (true);
    
    close(sockfd);

    return 0;
}