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
	// fclose(this->resource_p);
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
	ret += pwd;
	ret +=  "/";
	ret += loc_root[0] == '.' ? loc_root.substr(1) : loc_root;
	ret += req_path;//.substr(location.getRootedDir().size()); //location.get_path().size()
	free (pwd);
	std::cerr << "this is the full path: " << ret << std::endl;
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

CGI::~CGI() { 
	// if (this->resource_p) {
	// 	fclose(this->resource_p);
	// }
};

int	CGI::execute_CGI(HttpRequest& httprequest, Location& location, Server& server) {
	int	read_fd[2];
	int	write_fd;
	std::string filename = ".tmp";
	FILE* file = NULL;
	// int	pid;
	write_fd = write_to_CGI(filename, file);
	// std::cerr << "this is execute_CGI" << std::endl;
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
		//setting env in child process so no need to free
		char **env = set_env();
		char *av[3];
		av[0] = strdup("/usr/bin/php");
		av[1] = strdup(this->get_target_file_fullpath(httprequest, location).c_str());
		av[2] = NULL;
		std::cerr << "before execve in execute_CGI" << std::endl;
		execve(av[0], av, env);
		std::cerr << "after execve in execute_CGI" << std::endl;
		exit(1);
	} else {
		
		std::cout << "Parent inside execute_CGI" << std::endl;

		close(write_fd);
		close(read_fd[1]);
		int status;
		waitpid(pid, &status, 0);
		char buffer[BUFSIZ];
    	ssize_t bytesRead;

		std::cerr << "reading the file!" << std::endl;
    	// Read from the file descriptor until EOF is reached
    	while ((bytesRead = read(write_fd, buffer, sizeof(buffer))) > 0) {
    	    // Write the read data to stdout
    	    ssize_t bytesWritten = write(STDOUT_FILENO, buffer, bytesRead);
    	    if (bytesWritten != bytesRead) {
    	        std::cerr << "Error writing to stdout" << std::endl;
    	        return 1;
    	    }
    	}
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			std::cerr << "Error: cgi failed" << std::endl;
			return -1;
		}
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
    while ((read_bytes = read(this->get_read_fd(), buf, CGI_READ_BUFFER)) > 0) {
        ret.append(buf, buf + read_bytes);
    }
	close(this->get_read_fd());
    if (read_bytes < 0) {
        std::cout << "Error: CGI -> Read failed.\n";
        return "NO file";  // Return an empty string on error
    }

    // std::cout << "Read from CGI: " << ret << std::endl;
    return ret;
}

#include <fstream>
#include <string>
#include <iostream>
int	CGI::write_to_CGI(const std::string& filenaem, FILE*& file) {

	// std::cerr << "this is the filenaem: " << filenaem << std::endl;
	int tmp_file = open(filenaem.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
	std::cerr << "...........this is the file_resource: " << this->file_resource << std::endl;
	if (tmp_file != -1) {
		std::cerr << "File created: " << filenaem << " with content: " << file_resource.c_str() << " size: " << file_resource.length() << std::endl;
		write(tmp_file, file_resource.c_str(), file_resource.length() + 1);
		std::cerr << "File writen: " << file_resource.c_str() << std::endl;
		lseek(tmp_file, 0, SEEK_SET);
		return tmp_file;
	} else {
		std::cerr << "Write to CGI file failed" << std::endl;
		return -1;
	}

	// file = std::fopen(filenaem.c_str(), "r");
	// std::cerr << "this is the file: " << fileno(file) << " || " << file << std::endl;
	
	// int c;
	// std::cerr << "------this is the file: ******" << std::endl;

	// while ((c = fgetc(file)) != EOF) {
	// 	std::cerr.put(c);
	// }
	// if (file != NULL) {
	// 	return (fileno(file));
	// } else {
	// 	std::cerr << "Write to CGI file failed" << std::endl;
	// 	return -1;
	// }
}