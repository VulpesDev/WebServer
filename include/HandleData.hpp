#ifndef HANDLEDATA_HPP
# define HANDLEDATA_HPP

# include <sys/stat.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <ctime>
# include <string>
# include <fcntl.h>
# include <fstream>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>

# include <CGI.hpp>
# include <ServerConfig.hpp>
# include <HttpMessage.hpp>

class HTTPResponse;
class HttpRequest;

# define MAX_EVENTS 4096
# define TIMEOUT_SEC 10 //5-10 seconds is apparently common according to chatGPT 
# define DEFAULT_PATH "data/www"

bool        check_method_access(Server server, std::string path, std::string method);
std::string handle_request_checks(Server& server, HttpRequest& req);

std::string handle_get_request(const Server server, const std::string& resource_path);
std::string handle_post_request(const Server server, const std::string& resource_path, const std::string& file_content);
std::string handle_delete_request(const Server server, const std::string& resource_path);
void        handle_data(int fd, std::string port, std::vector<Server> serverconfs);

std::string                     check_error_page(Server server, std::string path, int error_code);
std::pair<std::string, ssize_t> receive_all(int client_fd, std::string port, std::vector<Server> server_confs, Server& server);

#endif
