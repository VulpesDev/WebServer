/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtimsina <rtimsina@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 20:00:15 by rtimsina          #+#    #+#             */
/*   Updated: 2024/02/16 20:00:15 by rtimsina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <CGI.hpp>
#include "../include/CGI.hpp"
#include <sstream>
#include <iostream>


template <typename T>
std::string NumberToString(T number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

#define CGI_RESOURCE_BUFFER 100
#define CGI_READ_BUFFER 64000

/*
# meta-variables
content_type		: "text/html"
gateway_interface 	: CGI/1.1
path_info			: /path/bla.cgi
path_translated		: /user/fullpath/to/cgi/file (get_target_file_full_path)
query_string		: "param1=value1&param2=value2"
req_method			: GET / POST
content_len			: 0
req_uri				: /path/bla.cgi
req_ident			:
redirect_status		: 200
remote_host			: "example.com" (value from request.headers["Host"])
romote_addr			: 192.168.1.1 (reault of get_ip(request.get_client_fd()))
script_name			: /path/bla.cgi
script_filename		: /user/path/to/cgi/file (get_target_file_fullpath)
server_name			: localhost (request.headers["Host"])
server_port			: 8080  (value from request.get_port())
server_protocal		: HTTP/1.1
server_software		: webserv/1.0
*/

static void set_signal_kill_child_process(int sig) {
	(void) sig;
	kill(-1, SIGKILL);
}

CGI::CgiHandler(std::string request, std::string location) {
	this->env["AUTH_TYPE"] = "";
	this->env["CONTENT_TYPE"] = "text/html"; //get from httprequest.headers["content_type"]
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["PATH_INFO"] = "/cgi-bin/bla.cgi"; //httprequest.get_path()
	this->env["PATH_TRANSLATED"] = "rtimsina/webserv/cgi-bin/cgi.bla"; //get_target_file_fullpath(httprequest, location)
	this->env["QUERY_STRING"] = "param1=value1&param2=value2"; //httprequest.get_query()
	this->env["REMOTE_HOST"] = "example.com"; //httprequest.headers["Host"]
	this->env["REMOTE_ADDR"] = "192.168.1.1"; //get_ip(httprequest.get_client_fd())
	this->env["REMOTE_USER"] = "";
	this->env["REMOTE_IDENT"] = "";
	this->env["REQUEST_METHOD"] = "GET"; //httprequest.get_method()
	this->env["REQUEST_URI"] = "/cgi-bin/bla.cgi"; //httprequest.get_path()
	this->env["SCRIPT_NAME"] = "/cgi-bin/bla.cgi"; //httprequest.get_path()
	this->env["SCRIPT_FILENAME"] = "rtimsina/webserv/cgi-bin/cgi.bla"; //get_target_file_fullpath(httprequest, location)
	this->env["SERVER_NAME"] = "localhost"; //httprequest.headers["Host"]
	this->env["SERVER_PROTOCAL"] = "HTTP/1.1";
	this->env["SERVER_PORT"] = "8080"; //httprequest.get_port()
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["CONTENT_LENGTH"] = "-1";
	load_file_resource(std::string request);
}

void	CGI::load_file_resource(std::string httprequest) {
	if (httprequest == "GET") { //httprequest.getmethod()
		this->resource_p = fopen(this->env["PATH_TRANSLATED"].c_str(), "rb");
		char buffer[CGI_RESOURCE_BUFFER + 1];
		memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		int read = 1;
		while ((read = fread(buffer, 1, CGI_RESOURCE_BUFFER, this->resource_p)) > 0) {
			this->file_resource += buffer;
			memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		}
		this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
	}
	if (httprequest == "POST") {
		this->file_resource = httprequest; //httprequest body
		this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
	}
}

std::string CGI::get_target_file_fullpath(std::string httprequest, std::string location) {
	std::string ret;
	char *pwd = getcwd(NULL, 0);
	std::string loc_root = location; // location.get_root();
	std::string req_path = httprequest; // httprequest.get_path()

	ret += pwd;
	ret += loc_root[0] == '.' ? loc_root.substr(1) : loc_root;
	ret += req_path.substr(location.size()); //location.get_path().size()
	free (pwd);
	return ret;

	/* Suppose:
	Current working directory is "/home/user".
	loc_root is "../htdocs".
	req_path is "/example/file.html".
	loc.get_path() returns "/example".
	After executing the function, the resulting ret would be 
	"/home/user/htdocs/file.html". The function constructs the full file path by combining 
	the current working directory, the adjusted loc_root, and the modified req_path. */
}

char**	CGI::set_env(void) {
	char **envp = new char*[env.size() + 1];
	if (!envp) {
		return NULL;
	}
	int	i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
		envp[i] = strdup((it->first + "=" it->second).c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

int	CGI::execute_CGI(std::string httprequest, std::string location) {
	int	read_fd[2];
	int	write_fd[2];
	int	pid;
	

	if (pipe(read_fd) < 0 || pipe(write_fd) < 0 || (httprequest == "GET" && !resource_p)) {
		return -1;
	}
	signal(SIGALRM, set_signal_kill_child_process);
	pid = fork();
	if (pid < 0) {
		return -1;
	} else if (pid == 0) {
		dup2(write_fd[0], STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		close(write_fd[0]);
		close(write_fd[1]);
		close(read_fd[0]);
		close(read_fd[1]);
		char **env = set_env(void);
		char *av[3];
		av[0] = strdup(location).c_str(); //executable path cgi-bin/cgi.bla
		av[1] = strdup(location).c_str(); //root location www/html
		av[2] = NULL;
		execve(av[0], av, env);
		exit(1);
	} else {
		close(write_fd[0]);
		close(read_fd[1]);
		set_write_fd(write_fd[1]);
		set_read_fd(read_fd[0]);
		return 0;
	}
}

std::string& CGI::get_file_resource(void) {
	return this->file_resource;
}

int	CGI::get_write_fd(void) {
	return this->_write_fd;
}

int	CGI::get_read_fd(void) {
	return this->_read_fd;
}

void CGI::set_write_fd(int fd) {
	this->_write_fd = fd;
}

void CGI::set_read_fd(int fd) {
	this->_read_fd = fd;
}

std::string CGI::read_from_CGI(void) {
	int read_bytes = 1;
	char buf[CGI_READ_BUFFER + 1];
	// memset(buf, 0, CGI_READ_BUFFER + 1);
	std::string ret;

	while (read_bytes > 0) {
		read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER);
		if (read_bytes < 0) {
			return NULL:
		}
		ret.resize(ret.size() + read_bytes);
		std::copy(buf.begin(), buf.begin() + read_bytes, ret.end() - read_bytes);
		// ret += buf;
		// memset(buf, 0, CGI_READ_BUFFER + 1);
	}
	// ret[ret.size() - 1] = '\0';
	return ret;
}

int	CGI::write_to_CGI(void) {

	int	wByte = write(this->get_write_fd(), this->file_resource.c_str(), this->file_resource.size());
	if (wByte < 0) {
		std::cout << "ERROR\n: CGI -> Write failed.\n";
		alarm(30);
		waitpid(-1, NULL, 0);
		return -1;
	} else {
		signal(SIGALRM, SIG_DFL);
		return wByte;
	}
}