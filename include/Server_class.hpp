#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>

class Server;
# include <ServerConfig.hpp>

# define DEFAULT_PORT 80
# define DEFAULT_BODYSIZE 0
# define DEFAULT_SERVERNAME "default"

# define PORT_VAL "listen" //def listen
# define ERR_PAGE_VAL "error_page" //def error_page
# define SERV_NAME_VAL "server_name" //def server_name
# define CLIENT_BODY_SIZE_VAL "client_max_body_size" //def client_max_body_size

class Server
{
	private:
		int									fd;
		std::string							port;
		static bool 						dhp_set;
		errPages_arr						err_pages;
		std::vector<std::string>			server_name;
		int									max_body_size;
		static std::pair<std::string, int>	default_host_port;
		
		
		
		int		servName_validate_fill(otherVals_itc it);
		int		host_port_validate_fill(otherVals_itc it);
		int		errorPages_validate_fill(otherVals_itc it);
		int		maxBodySize_validate_fill(otherVals_itc it);

	public:

		Server();
		Server( Server const & src );
		~Server();

		Server &		operator=( Server const & rhs );

		void	mapToValues( void );
		void	printValues( void ) const;
		otherVals_map						other_vals;
		std::vector<Location>				locations;

		int getFd() const { return fd; }
		void setFd(int newFd) { fd = newFd; }
		std::string					GetPort() const;
		void						SetPort(std::string newPort) { port = newPort; };
		int							GetMaxBodySize() const;
		// bool						GetDhpSet();
		errPages_arr				GetErrPages() const;
		std::vector<std::string>	GetServNames() const;
		// std::pair<std::string, int>	GetDefHostPort();

		//////// Exception ////////

		class ServerName_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing server name";
				}
		};
		class BodySize_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing server body size";
				}
		};
		class BodySizeNnumval_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing server body size value";
				}
		};
		class BodySizeUnit_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing server body size value unit";
				}
		};
		class PortWrongParam_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing server port";
				}
		};
		class ErrorPageFile_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing error page file";
				}
		};
		class ErrorPageNotNumeric_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing error pages, value is not numeric";
				}
		};
		class ErrorPageLessZero_Exception : public std::exception {
			public :
				const char* what() const throw(){
					return "while parsing error pages, value is less than zero";
				}
		};
		class UnrecognisedCommandException : public std::exception {
			public :
				const char* what() const throw(){
					return "unrecognised command";
				}
		};
		class NumberOverflowException : public std::exception {
			public :
				const char* what() const throw(){
					return "number is either too big or too small";
				}
		};
		class ErrorPageNotNumericException : public std::exception {
			public :
				const char* what() const throw(){
					return "error pages: not numeric value";
				}
		};
		class ErrorPageErrorException : public std::exception {
			public :
				const char* what() const throw(){
					return "error pages: error";
				}
		};
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ********************************************** Server_H */
