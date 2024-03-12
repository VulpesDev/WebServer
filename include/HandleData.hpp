#include <iostream>
#include <sstream>
#include <string>

#include "../include/CGI.hpp"
#include "../include/HttpMessage.hpp"
//check if the method has .php extension, based on this cgi will be executed
#include "../include/HttpMessage.hpp"
#include <HttpMessage.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctime>
#include <ServerConfig.hpp>
#include "../include/HttpMessage.hpp"
#define MAX_EVENTS 4096
#define TIMEOUT_SEC 10 //5-10 seconds is apparently common according to chatGPT 
#include <fstream>
#include <fcntl.h>

std::string handle_get_request(const std::string& resource_path);
std::string handle_delete_request(const std::string& resource_path);
std::string handle_post_request(const std::string& resource_path, const std::string& file_content);
void handle_data(int client_fd);