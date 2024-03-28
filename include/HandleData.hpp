#include <iostream>
#include <sstream>
#include <string>

#include "HttpMessage.hpp"
#include "../include/CGI.hpp"
//check if the method has .php extension, based on this cgi will be executed
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
#define MAX_EVENTS 4096
#define TIMEOUT_SEC 2 //5-10 seconds is apparently common according to chatGPT 
#define DEFAULT_PATH "data/www"
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

std::string handle_get_request(const Server server, const std::string& resource_path);
std::string handle_delete_request(const Server server, const std::string& resource_path);
std::string handle_post_request(const Server server, const std::string& resource_path, const std::string& file_content);
void handle_data(int fd, std::string port, std::vector<Server> serverconfs);
std::string check_error_page(Server server, std::string path, int error_code);