#include <CGI.hpp>
#include <sstream>
#include <iostream>

pid_t pid;

template <typename T>
std::string NumberToString(T number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}


static void set_signal_kill_child_process(int sig) {
	(void) sig;
	std::cerr << "Killing child process" << std::endl;
	kill(pid, SIGKILL);
}

CGI::CGI(HttpRequest& request, Location& location, Server& server) {
	
	std::cerr << "Loading env variables" << std::endl;


	this->env["AUTH_TYPE"] = "";
	const std::map<std::string, std::string>& headers = request.getHeaders();

	this->env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers.at("Content-Type") : "text/html";
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->env["PATH_INFO"] = request.getPath();
	this->env["PATH_TRANSLATED"] = this->get_target_file_fullpath(request, location);
	this->env["QUERY_STRING"] = request.get_query();
	this->env["REQUEST_METHOD"] = request.getMethod();
	this->env["REQUEST_URI"] = request.getPath();
	this->env["SCRIPT_NAME"] = request.getPath();
	this->env["SCRIPT_FILENAME"] = this->get_target_file_fullpath(request, location);
	this->env["SERVER_NAME"] = request.getHeaders().count("Host") ? request.getHeaders().at("Host") : "localhost";
	std::cerr << "SERVER_NAME: " << this->env["SERVER_NAME"] << std::endl;
	this->env["SERVER_PROTOCAL"] = "HTTP/1.1";
	this->env["SERVER_SOFTWARE"] = "webserv/1.0";
	this->env["CONTENT_LENGTH"] = "-1";

	load_file_resource(request, server);
}

void	CGI::load_file_resource(HttpRequest& httprequest, Server& server) {
	(void)server;
	if (httprequest.getMethod() == "GET") {
		this->resource_p = fopen(this->env["PATH_TRANSLATED"].c_str(), "rb");
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
		this->file_resource.append(httprequest.getBody());
		this->env["CONTENT_LENGTH"] = NumberToString(this->file_resource.size());
	}
}

std::string CGI::get_target_file_fullpath(HttpRequest httprequest, Location location) {
	std::string ret;
	char *pwd = getcwd(NULL, 0);
	std::string loc_root = location.getRootedDir();
	std::string req_path = httprequest.getPath();
	ret += pwd;
	ret +=  "/";
	ret += loc_root[0] == '.' ? loc_root.substr(1) : loc_root;
	ret += req_path;
	free (pwd);
	return ret;

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

CGI::~CGI() {}

std::list<int> child_pids;

int CGI::nonblocking_waitpid(int pid) {
		int status = 0;

		std::cerr << "adding pid: " << pid << std::endl;
		child_pids.push_back(pid);
    	for (std::list<int>::iterator it = child_pids.begin(); it != child_pids.end(); ++it) {
    	    std::cout << "PID: " << *it << std::endl;
			waitpid(*it, &status, WNOHANG);
			if (WEXITSTATUS(status) != 0) {
				std::cerr << "Error: cgi failed" << std::endl;
				return -1;
			}
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0 && pid != 0) {
				std::cerr << "removing pid: " << *it << std::endl;
				it = child_pids.erase(it);
			}
    	}
		return 0;
}

int	CGI::execute_CGI(HttpRequest& httprequest, Location& location, Server& server) {
	(void)server;
	int	read_fd[2];
	int	write_fd;
	std::string filename = ".tmp";
	FILE* file = NULL;
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
	alarm(5);
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
		char **env = set_env();
		char *av[3];
		av[0] = strdup("/usr/bin/php");
		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
		av[2] = NULL;
		execve(av[0], av, env);
		exit(1);
	} else {
		
		close(write_fd);
		close(read_fd[1]);
		// int status;
		//nonblocking_waitpid(pid);
		signal(SIGALRM, SIG_DFL);
		alarm(0);
	}
	if (std::remove(filename.c_str()) != 0) {
		std::cerr << "Error: remove failed" << std::endl;
		return -1;
	}
	set_read_fd(read_fd[0]);
	return 0;
}

std::string& CGI::get_file_resource(void) {
	return this->file_resource;
}


int	CGI::get_read_fd(void) {
	return this->_read_fd;
}

void CGI::set_read_fd(int fd) {
	this->_read_fd = fd;
}

std::string CGI::read_from_CGI() {
    char buf[CGI_READ_BUFFER];
    std::string ret;

    int read_bytes;
    while ((read_bytes = recv(this->get_read_fd(), buf, CGI_READ_BUFFER, MSG_DONTWAIT)) > 0) {
        ret.append(buf, buf + read_bytes);
    }
	close(this->get_read_fd());
    if (read_bytes < 0) {
		if (read_bytes == EAGAIN || EWOULDBLOCK) {
			return ret; // read later
		}
        std::cout << "Error: CGI -> Read failed.\n";
        return "";
    }
    return ret;
}

int	CGI::write_to_CGI(const std::string& filenaem, FILE*& file) {
	(void)file;
	int tmp_file = open(filenaem.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (tmp_file != -1) {
		write(tmp_file, file_resource.c_str(), file_resource.length() + 1);
		lseek(tmp_file, 0, SEEK_SET);
		return tmp_file;
	} else {
		std::cerr << "Write to CGI file failed" << std::endl;
		return -1;
	}
}
