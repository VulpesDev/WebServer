 
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

pid_t pid;

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
	std::cerr << "Killing child process" << std::endl;
	kill(pid, SIGKILL);
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

// static int kill_program() {
// 	int time = 0;
// 	while (time < 6) {
// 		time++;
// 		sleep(1);
// 	}
// 	return time;
// }
int	CGI::execute_CGI(HttpRequest& httprequest, Location& location, Server& server) {
	int	read_fd[2];
	int	write_fd;
	std::string filename = ".tmp";
	FILE* file = NULL;
	// int	pid;
	write_fd = write_to_CGI(filename, file);
	if (write_fd < 0) {
		std::cerr << "Error: write_to_CGI failed" << std::endl;
		return -1;
	}
	if (pipe(read_fd) < 0 || (httprequest.getMethod() == "GET" && !resource_p)) {
		std::cerr << "Error: pipe failed" << std::endl;
		return -1;
	}
	signal(SIGALRM, set_signal_kill_child_process);
	alarm(20);
	pid = fork();
	if (pid < 0) {
		std::cerr << "Error: fork failed" << std::endl;
		return -1;

	} else if (pid == 0) {
		signal(SIGALRM, SIG_DFL);
		alarm(0);
		dup2(write_fd, STDIN_FILENO);
		close(write_fd);

		dup2(read_fd[1], STDOUT_FILENO);
		close(read_fd[1]);
		close(read_fd[0]);
		//setting env in child process so no need to free
		char **env = set_env();
		char *av[3];
		av[0] = strdup("/usr/bin/php");
		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
		av[2] = NULL;
		std::cerr << "before execve in execute_CGI" << std::endl;
		// alarm(5);
		execve(av[0], av, env);
		std::cerr << "after execve in execute_CGI" << std::endl;
		exit(1);
	} else {
		
		std::cout << "Parent inside execute_CGI" << std::endl;

		close(write_fd);
		close(read_fd[1]);
		int status;
		waitpid(pid, &status, 0);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			std::cerr << "Error: cgi failed" << std::endl;
			return -1;
		}
	}
	if (std::remove(filename.c_str()) != 0) {
		std::cerr << "Error: remove failed" << std::endl;
		return -1;
	}
	set_read_fd(read_fd[0]);
	return 0;
}


//different approach

// int	CGI::execute_CGI(HttpRequest& httprequest, Location& location, Server& server) {
// 	int	read_fd[2];
// 	int	write_fd[2];
// 	// int	pid;
	
// 	if (pipe(read_fd) < 0 || pipe(write_fd) < 0 || (httprequest.getMethod() == "GET" && !resource_p)) {
// 		return -1;
// 	}
// 	// signal(SIGALRM, set_signal_kill_child_process);
// 	// alarm(5);
// 	pid = fork();
// 	if (pid < 0) {
// 		return -1;

// 	} else if (pid == 0) {
// 		// signal(SIGALRM, SIG_DFL);
// 		// alarm(0);
// 		dup2(write_fd[0], STDIN_FILENO);
// 		dup2(read_fd[1], STDOUT_FILENO);
// 		close(write_fd[0]);
// 		close(write_fd[1]);
// 		close(read_fd[0]);
// 		close(read_fd[1]);
// 		char **env = set_env();
// 		char *av[3];
// 		av[0] = strdup("/usr/bin/php");
// 		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
// 		av[2] = NULL;
// 		std::cerr << "before execve in execute_CGI" << std::endl;
// 		// alarm(5);
// 		execve(av[0], av, env);
// 		std::cerr << "after execve in execute_CGI" << std::endl;
// 		exit(1);
// 	} else {
// 		// waitpid(pid, NULL, 0);

// 		// int time = kill_program();
// 		// if (time >= 6) {
// 		// 	std::cout << "cgi response build failed, kill program timeout" << std::endl;
// 		// 	// check_error_page(server, request.getPath(), 502);
// 		// 	return -1;
// 		// }
// 		// signal(SIGALRM, set_signal_kill_child_process);
// 		// alarm(5);
// 		// int status;
//         // pid_t result = waitpid(pid, &status, 0);
//         // if (result == -1) {
//         //     std::cerr << "Error waiting for child process" << std::endl;
//         //     return -1;
//         // } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
//         //     std::cerr << "Child process terminated due to timeout" << std::endl;
//         //     return -1;
//         // }
//         // alarm(0);	

// 		std::cout << "Parent inside execute_CGI" << std::endl;
// 		close(write_fd[0]);
// 		close(read_fd[1]);
// 		set_write_fd(write_fd[1]);
// 		set_read_fd(read_fd[0]);

// 		return 0;
// 	}
// 	// return 0;
// }


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

// std::string CGI::read_from_CGI(int fd) {
//     char buf[CGI_READ_BUFFER];
//     std::string ret;

//     int read_bytes;
//     while ((read_bytes = read(fd, buf, CGI_READ_BUFFER)) > 0) {
//         ret.append(buf, buf + read_bytes);
//     }

//     if (read_bytes < 0) {
//         std::cout << "Error: CGI -> Read failed.\n";
//         return "NO file";  // Return an empty string on error
//     }

//     std::cout << "Read from CGI: " << ret << std::endl;
//     return ret;
// }
std::string CGI::read_from_CGI() {
    char buf[CGI_READ_BUFFER];
    std::string ret;

    int read_bytes;
    while ((read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER)) > 0) {
        ret.append(buf, buf + read_bytes);
    }
	close(this->get_read_fd());
    if (read_bytes < 0) {
        std::cout << "Error: CGI -> Read failed.\n";
        return "NO file";  // Return an empty string on error
    }

    std::cout << "Read from CGI: " << ret << std::endl;
    return ret;
}


int	CGI::write_to_CGI(const std::string& filenaem, FILE*& file) {

	std::ofstream tmp_file(filenaem.c_str());
	if (tmp_file.is_open()) {
		tmp_file << this->file_resource;
		tmp_file.close();
		return 0;
	} else {
		std::cerr << "Write to CGI file failed" << std::endl;
		return -1;
	}

	file = std::fopen(filenaem.c_str(), "r");
	if (file != NULL) {
		return (fileno(file));
	} else {
		std::cerr << "Write to CGI file failed" << std::endl;
		return -1;
	}
}
// int	CGI::write_to_CGI(void) {

// 	int	wByte = write(this->get_write_fd(), this->file_resource.c_str(), this->file_resource.size());
// 	if (wByte < 0) {
// 		std::cout << "ERROR\n: CGI -> Write failed.\n";
// 		alarm(30);
// 		waitpid(-1, NULL, 0);
// 		return -1;
// 	} else {
// 		signal(SIGALRM, SIG_DFL);
// 		return wByte;
// 	}
// }