#ifndef SERVERCONFIG_CLASS_HPP
# define SERVERCONFIG_CLASS_HPP

# include <iostream>
# include <string>

class ServerConfig_class;
# include <ServerConfig.hpp>

# define DEFAULT_PORT 80
# define DEFAULT_BODYSIZE 0
# define DEFAULT_SERVERNAME "default"

# define PORT_VAL "listen" //def listen
# define ERR_PAGE_VAL "error_page" //def error_page
# define SERV_NAME_VAL "server_name" //def server_name
# define CLIENT_BODY_SIZE_VAL "client_max_body_size" //def client_max_body_size

class ServerConfig_class
{

	public:

		ServerConfig_class();
		ServerConfig_class( ServerConfig_class const & src );
		~ServerConfig_class();

		ServerConfig_class &		operator=( ServerConfig_class const & rhs );

		void	mapToValues( void );
		void	printValues( void );
		otherVals_map						other_vals;
		std::vector<LocationConfig_class>	locations;

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
		
	private:
		int									port;
		static bool 						dhp_set;
		errPages_arr						err_pages;
		std::vector<std::string>			server_name;
		int									max_body_size;
		static std::pair<std::string, int>	default_host_port;
		
		
		
		int		servName_validate_fill(otherVals_itc it);
		int		host_port_validate_fill(otherVals_itc it);
		int		errorPages_validate_fill(otherVals_itc it);
		int		maxBodySize_validate_fill(otherVals_itc it);

};

std::ostream &			operator<<( std::ostream & o, ServerConfig_class const & i );

#endif /* ********************************************** SERVERCONFIG_CLASS_H */
