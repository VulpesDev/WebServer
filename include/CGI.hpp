#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>
# include <iostream>
# include <string>
# include <cstring>
# include <map>
# include "HttpMessage.hpp"
# include "Location_class.hpp"
# include "HandleData.hpp"

# define BUFFER_SIZE 1024
# define CGI_RESOURCE_BUFFER 100
# define CGI_READ_BUFFER 1024

class HttpRequest;

class CGI {
	private:
		std::map<std::string, std::string> env;
		FILE	*resource_p;
		std::string file_resource;
		int	_write_fd;
		int	_read_fd;

	public:
		CGI(HttpRequest& request, Location& location, Server& server);
		~CGI();
		char **set_env();
		int	execute_CGI(HttpRequest& request, Location& location, Server& server);
		void printCgiEnvironment(std::ostream &out) const {
			out << "cgi_env\n";
			for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
				out << "first : " << it->first << " || second : " << it->second << '\n';
			}
		};
		
		std::string get_target_file_fullpath(HttpRequest request, Location location);
		std::string &get_file_resource(void);

		int	get_read_fd(void);
		void	set_read_fd(int fd);
		void	load_file_resource(HttpRequest& request, Server& server);
		std::string read_from_CGI(void);
		int		write_to_CGI(const std::string& filenaem, FILE*& file);
};

inline std::ostream &operator<<(std::ostream &out, const CGI &ch) {
	ch.printCgiEnvironment(out);
	return out;
}

#endif