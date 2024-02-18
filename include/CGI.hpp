/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtimsina <rtimsina@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 19:37:28 by rtimsina          #+#    #+#             */
/*   Updated: 2024/02/16 19:37:28 by rtimsina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <cstring>
#include <map>

using namespace std;

#define BUFFER_SIZE 1024

class CGI {
	private:
		std::map<std::string, std::string> env;
		FILE	*resource_p;
		std::string file_resource;
		int	_write_fd;
		int	_read_fd;

	public:
		CGI(std::string request, std::string location);
		//request and location should be from their class respectively
		char **set_env();
		int	execute_CGI(std::string request, std::string location);
		//request and location should be from their class respectively
		void printCgiEnvironment(std::ostream &out) const {
			out << "cgi_env\n";
			for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
				out << "first : " << it->first << " || second : " << it->second << '\n';
			}
		};
		
		std::string get_target_file_fullpath(std::string request, std::string location);
		//request and location should be from their class respectively
		std::string &get_file_resource(void);

		int	get_write_fd(void);
		int	get_read_fd(void);
		void	set_write_fd(int fd);
		void	set_read_fd(int fd);
		void	load_file_resource(std::string request);
		//request is a http request class
		std::string read_from_CGI(void);
		int		write_to_CGI(void);

};

inline std::ostream &operator<<(std::ostream &out, const CGI &ch) {
	ch.printCgiEnvironment(out);
	return out;
}

#endif