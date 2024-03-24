 
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

CGI::CGI(HttpRequest& request, Location& location, Server& server) {
	
	std::cerr << "Loading env variables" << std::endl;


	this->env["AUTH_TYPE"] = "";
	//this->env["CONTENT_TYPE"] = headers["Content-Type"];

	const std::unordered_map<std::string, std::string>& headers = request.getHeaders();
	// std::cerr << "Loading env variables Content type" << std::endl;
	this->env["CONTENT_TYPE"] = "text/html";
	// this->env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers.at("Content-Type") : "";
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// std::cerr << "1. Loading env variables Content type" << std::endl;
	this->env["PATH_INFO"] = request.getPath(); //"/data/www/basic.php"; //httprequest.get_path()
	// std::cerr << "2. Loading env variables Content type" << std::endl;
	this->env["PATH_TRANSLATED"] = this->get_target_file_fullpath(request, location);
	// std::cerr << "3. Loading env variables Content type" << std::endl;
	// this->env["PATH_TRANSLATED"] = "/home/ramesh/Documents/my_webserv/data/www/basic.php"; //get_target_file_fullpath(httprequest, location)
	this->env["QUERY_STRING"] = request.get_query(); //"param1=value1&param2=value2"; //httprequest.get_query()
	// std::cerr << "4. Loading env variables Content type" << std::endl;
	this->env["REMOTE_HOST"] = ""; //httprequest.headers["Host"]
	this->env["REMOTE_ADDR"] = ""; //get_ip(httprequest.get_client_fd())
	this->env["REMOTE_USER"] = "";
	this->env["REMOTE_IDENT"] = "";
	this->env["REQUEST_METHOD"] = request.getMethod();
	// std::cerr << "5. Loading env variables Content type" << std::endl;
	this->env["REQUEST_URI"] = request.getPath();// maybe check with print statement. //httprequest.get_path()
	this->env["SCRIPT_NAME"] = request.getPath();
	this->env["SCRIPT_FILENAME"] = this->get_target_file_fullpath(request, location);
	// this->env["SCRIPT_FILENAME"] = "/home/ramesh/Documents/my_webserv/data/www/basic.php"; //get_target_file_fullpath(httprequest, location)
	this->env["SERVER_NAME"] = "Spyder"; //httprequest.headers["Host"]
	this->env["SERVER_PROTOCAL"] = "HTTP/1.1";
	// this->env["SERVER_PORT"] = "8080"; //httprequest.get_port()
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["CONTENT_LENGTH"] = "-1";
	//print all the env variables with std::cerr
	// for (auto const& env_var : this->env) {
	// 	std::cerr << env_var.first << " : " << env_var.second << std::endl;
	// }
	
	
	// std::cout << "Loaded env variables" << std::endl;
	load_file_resource(request, server);
}

void	CGI::load_file_resource(HttpRequest& httprequest, Server& server) {
	std::cerr << "Loading File resource" << std::endl;
	if (httprequest.getMethod() == "GET") {
		this->resource_p = fopen(this->env["PATH_TRANSLATED"].c_str(), "rb");
		//check if filepointer needs to be closed
		if (this->resource_p == NULL) {
			std::cerr << "File not found" << std::endl;
			// generate_error_page(404);
			check_error_page(server, httprequest.getPath(), 404);
			return ;
		}
		char buffer[CGI_RESOURCE_BUFFER + 1];
		memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		// std::cout << "Memset ins Load File resource" << std::endl;
		int read = 1;
		// int counter = 1;
		while ((read = fread(buffer, 1, CGI_RESOURCE_BUFFER, this->resource_p)) > 0) {
			this->file_resource += buffer;
			// counter++;
			// std::cout << "Counter: " << counter << std::endl;
			memset(buffer, 0, CGI_RESOURCE_BUFFER + 1);
		}
		// std::cout << "this is from fread from load_file_resource: " << this->file_resource << std::endl;
		// this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
		std::cerr << "Done Get Method" << std::endl;
	}
	if (httprequest.getMethod() == "POST") {
		this->file_resource = httprequest.getBody();
		std::cerr << "this is from getBody from load_file_resource: " << this->file_resource << std::endl;
		this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
	}
}

std::string CGI::get_target_file_fullpath(HttpRequest httprequest, Location location) {
	std::string ret;
	char *pwd = getcwd(NULL, 0);
	std::string loc_root = location.getRootedDir(); // location.get_root();
	std::string req_path = httprequest.getPath(); // httprequest.get_path()

	ret += pwd;
	ret +=  "/data/www";
	ret += loc_root[0] == '.' ? loc_root.substr(1) : loc_root;
	ret += req_path.substr(location.getRootedDir().size()); //location.get_path().size()
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
	// std::cout << "inside execute_CGI after pipe" << std::endl;
	signal(SIGALRM, set_signal_kill_child_process);
	// alarm(2);
	pid = fork();
	// std::cout << pid << std::endl;
	if (pid < 0) {
		return -1;

	} else if (pid == 0) {
		std::cerr << "Child inside execute_CGI" << std::endl;
		dup2(write_fd[0], STDIN_FILENO);
		dup2(read_fd[1], STDOUT_FILENO);
		// close(write_fd[0]);
		close(write_fd[1]);
		close(read_fd[0]);
		// close(read_fd[1]);
		std::cerr << "2. Child inside execute_CGI" << std::endl;
		char **env = set_env();
		char *av[3];
		av[0] = strdup("/usr/bin/php");
		// std::cout << av[0] << std::endl;
		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
		// av[1] = const_cast<char*>(location.getRootedDir().c_str()); //root location www/html
		// std::cout << av[1] << std::endl;
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


// #include <errno.h>
// #include <string.h>
// #include <sys/time.h>
// #include <unistd.h>

// // Define timeout duration in seconds
// // #define CGI_READ_TIMEOUT_SEC 30

// std::string CGI::read_from_CGI() {
//     char buf[CGI_READ_BUFFER];
//     std::string ret;

//     // Set up timeout for read operation
//     struct timeval timeout;
//     timeout.tv_sec = 2;
//     timeout.tv_usec = 0;

//     // Set read file descriptor for select
//     fd_set read_fds;
//     FD_ZERO(&read_fds);
//     FD_SET(this->get_read_fd(), &read_fds);

//     // Wait for data to be available on the read file descriptor or timeout
//     int ready = select(this->get_read_fd() + 1, &read_fds, NULL, NULL, &timeout);
//     if (ready == -1) {
//         // Error occurred in select
//         std::cerr << "Error in select: " << strerror(errno) << std::endl;
//         return "ERROR: Read failed";
//     } else if (ready == 0) {
//         // Timeout occurred
//         std::cerr << "Timeout: No data received from CGI script within " << 2 << " seconds." << std::endl;
//         return "ERROR: Read timeout";
//     }

//     // Read data from CGI script
//     int read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER);
//     if (read_bytes > 0) {
//         ret.append(buf, buf + read_bytes);
//         std::cout << "Read from CGI: " << ret << std::endl;
//         return ret;
//     } else if (read_bytes == 0) {
//         // End of file reached
//         return ret;
//     } else {
//         // Error occurred in read
//         std::cerr << "Error in read: " << strerror(errno) << std::endl;
//         return "ERROR: Read failed";
//     }
// }



int	CGI::write_to_CGI(void) {

	int	wByte = write(this->get_write_fd(), this->file_resource.c_str(), this->file_resource.size());
	// this is working with the file and control is comming here.
	// std::cout << this->get_write_fd() << " This is write_to_CGI: " << this->file_resource.c_str() << std::endl;
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



// std::string CGI::read_from_CGI(int fd) {


// 	char *buf = new char[4096];
// 	memset(buf, 0, 4096);
// 	if (read(fd, buf, 4096) < 0) {
// 		std::cerr << "Error: CGI -> Read failed.\n";
// 		return "";  // Return an empty string on error
// 	}
// 	std::string ret(buf);
// 	delete[] buf;
// 	std::cout << "Read from CGI: " << ret << std::endl;
// 	return ret;
// }



// std::string CGI::read_from_CGI(void) {


// 	int read_bytes = 1;
// 	char buf[CGI_READ_BUFFER + 1];
// 	// memset(buf, 0, CGI_READ_BUFFER + 1);
// 	std::string ret;
// 	std::cerr << "read_from_CGI get read fd " << this->get_read_fd() << std::endl;

// 	while (read_bytes > 0) {
// 		read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER);
// 		if (read_bytes < 0) {
// 			return NULL;
// 		}
// 		ret.resize(ret.size() + read_bytes);
// 		std::copy(std::begin(buf), std::begin(buf) + read_bytes, ret.end() - read_bytes);
// 		// std::copy(buf.begin(), buf.begin() + read_bytes, ret.end() - read_bytes);
// 		// ret += buf;
// 		// memset(buf, 0, CGI_READ_BUFFER + 1);
// 	}
// 	std::cout << "read_from_CGI buf: " << buf << std::endl;
// 	// ret[ret.size() - 1] = '\0';
// 	std::cout << "read_from_CGI: " << ret << std::endl;
// 	// return this->file_resource.c_str();
// 	return ret;
// }

// int	CGI::write_to_CGI(const std::string& file_name, FILE*& file) {

// 	std::ofstream tmp_file(file_name.c_str());
// 	if (tmp_file.is_open()) {
// 		for (unsigned int i = 0; i < this->file_resource.size(); i++) {
// 			tmp_file << this->file_resource[i];
// 		}
// 		tmp_file.close();
// 	} else {
// 		return -1;
// 	}

// 	file = std::fopen(file_name.c_str(), "r");
// 	if (file != NULL) {
// 		return (fileno(file));
// 	} else {
// 		return -1;
// 	}
// }



// int	CGI::execute_CGI(HTTPRequest httprequest, std::string location) {
// 	int	read_fd[2];
// 	int	write_fd[2];
// 	int	pid;
	
// 	if (pipe(read_fd) < 0 || pipe(write_fd) < 0 || (httprequest.method == "GET" && !resource_p)) {
// 		return -1;
// 	}
// 	std::cout << "inside execute_CGI after pipe" << std::endl;
// 	signal(SIGALRM, set_signal_kill_child_process);
// 	pid = fork();
// 	std::cout << pid << std::endl;
// 	if (pid < 0) {
// 		return -1;
// 	} else if (pid == 0) {
// 		std::cerr << "Child inside execute_CGI" << std::endl;
// 		dup2(write_fd[0], STDIN_FILENO);
// 		dup2(read_fd[1], STDOUT_FILENO);
// 		std::cerr << "1. Child inside execute_CGI" << read_fd[1] << "read fd: " << write_fd[0] << "write fd: " << std::endl;
// 		close(write_fd[0]);
// 		close(write_fd[1]);
// 		// close(read_fd[0]);
// 		// close(read_fd[1]);
// 		std::cerr << "2. Child inside execute_CGI" << std::endl;
// 		char **env = set_env();
// 		char *av[3];
// 		av[0] = strdup("/home/rtimsina/Desktop/cursu_working/11_webserv/my_webserv/data/www/basic.php"); //executable path cgi-bin/cgi.bla
// 		// std::cout << av[0] << std::endl;
// 		av[1] = strdup(location.c_str()); //root location www/html
// 		// std::cout << av[1] << std::endl;
// 		av[2] = NULL;
// 		std::cerr << "before execve in execute_CGI" << std::endl;
// 		execve(av[0], av, env);
// 		exit(1);
// 	} else {
// 		// wait(NULL);
// 		std::cerr << "Parent inside execute_CGI" << std::endl;
// 		close(write_fd[0]);
// 		close(read_fd[1]);
// 		std::cerr << "1. Parent inside execute_CGI" << read_fd[0] << "read fd: " << write_fd[1] << "write fd: " << std::endl;
// 		set_write_fd(write_fd[1]);
// 		set_read_fd(read_fd[0]);

// 		return 0;
// 	}
// }