 
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

//from location class, we should only check for if cgi (fast_cgi) is enabled 
// if not never call the cgi handler

template <typename T>
std::string NumberToString(T number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

#define CGI_RESOURCE_BUFFER 100
#define CGI_READ_BUFFER 64000

static void set_signal_kill_child_process(int sig) {
	(void) sig;
	kill(-1, SIGKILL);
}

CGI::CGI(HttpRequest& request, Location& location, Server& server) {
	
	std::cerr << "Loading env variables" << std::endl;


	this->env["AUTH_TYPE"] = "";
	const std::unordered_map<std::string, std::string>& headers = request.getHeaders();

	this->env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers.at("Content-Type") : "text/html";
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["PATH_INFO"] = request.getPath(); //"/data/www/basic.php"; //httprequest.get_path()
	this->env["PATH_TRANSLATED"] = this->get_target_file_fullpath(request, location);
	this->env["QUERY_STRING"] = request.get_query(); //"param1=value1&param2=value2"; //httprequest.get_query()
	this->env["REMOTE_HOST"] = ""; //httprequest.headers["Host"]
	this->env["REMOTE_ADDR"] = ""; //get_ip(httprequest.get_client_fd())
	this->env["REMOTE_USER"] = "";
	this->env["REMOTE_IDENT"] = "";
	this->env["REQUEST_METHOD"] = request.getMethod();
	this->env["REQUEST_URI"] = request.getPath();// maybe check with print statement. //httprequest.get_path()
	this->env["SCRIPT_NAME"] = request.getPath();
	this->env["SCRIPT_FILENAME"] = this->get_target_file_fullpath(request, location);
	this->env["SERVER_NAME"] = "Spyder"; //httprequest.headers["Host"]
	this->env["SERVER_PROTOCAL"] = "HTTP/1.1";
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["CONTENT_LENGTH"] = "-1";

	load_file_resource(request, server);
}

void	CGI::load_file_resource(HttpRequest& httprequest, Server& server) {
	if (httprequest.getMethod() == "GET") {
		this->resource_p = fopen(this->env["PATH_TRANSLATED"].c_str(), "rb");
		//check if filepointer needs to be closed
		if (this->resource_p == NULL) {
			std::cerr << "File not found" << std::endl;
			return ;
		}
		char buffer[CGI_RESOURCE_BUFFER + 1];
		memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		int read = 1;
		while ((read = fread(buffer, 1, CGI_RESOURCE_BUFFER, this->resource_p)) > 0) {
			this->file_resource += buffer;
			memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		}
	}
	if (httprequest.getMethod() == "POST") {
		this->file_resource.append(httprequest.getBody()); //this = httprequest.getBody();
		std::cerr << "this is the size of body: " << this->file_resource.size() << std::endl;
		this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
	}
}

std::string CGI::get_target_file_fullpath(HttpRequest httprequest, Location location) {
	std::string ret;
	char *pwd = getcwd(NULL, 0);
	std::string loc_root = location.getRootedDir(); // location.get_root();
	std::string req_path = httprequest.getPath(); // httprequest.get_path()
	// std::cerr << "******* this is the req_path: " << req_path << std::endl;
	ret += pwd;
	ret +=  "/";
	ret += loc_root[0] == '.' ? loc_root.substr(1) : loc_root;
	// ret +=  "/cgi-bin";
	ret += req_path;//.substr(location.getRootedDir().size()); //location.get_path().size()
	free (pwd);
	std::cerr << "this is the full path: " << ret << std::endl;
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
		envp[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

int	CGI::execute_CGI(HttpRequest& httprequest, Location& location) {
	int	read_fd[2];
	int	write_fd[2];
	int	pid;
	
	if (pipe(read_fd) < 0 || pipe(write_fd) < 0 || (httprequest.getMethod() == "GET" && !resource_p)) {
		return -1;
	}
	signal(SIGALRM, set_signal_kill_child_process);
	// alarm(2);
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
		char **env = set_env();
		char *av[3];
		av[0] = strdup("/usr/bin/php");
		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
		av[2] = NULL;
		std::cerr << "before execve in execute_CGI" << std::endl;
		execve(av[0], av, env);
		exit(1);
	} else {
		// wait(NULL);
		std::cout << "Parent inside execute_CGI" << std::endl;
		close(write_fd[0]);
		close(read_fd[1]);
		set_write_fd(write_fd[1]);
		set_read_fd(read_fd[0]);

		// int status;
        // pid_t result = waitpid(pid, &status, 0);
        // if (result == -1) {
        //     std::cerr << "Error waiting for child process" << std::endl;
        //     return -1;
        // } else if (WIFSIGNALED(status)) {
        //     std::cerr << "Child process terminated due to timeout" << std::endl;
        //     return -1; // Handle timeout error here
        // }

        // Cancel the alarm after child process finishes
        // alarm(0);		
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

std::string CGI::read_from_CGI() {
    char buf[CGI_READ_BUFFER];
    std::string ret;

    int read_bytes;
    while ((read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER)) > 0) {
        ret.append(buf, buf + read_bytes);
    }

    if (read_bytes < 0) {
        std::cout << "Error: CGI -> Read failed.\n";
        return "NO file";  // Return an empty string on error
    }

    std::cout << "Read from CGI: " << ret << std::endl;
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