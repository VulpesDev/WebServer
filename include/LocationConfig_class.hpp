#ifndef LOCATIONCONFIG_CLASS_HPP
# define LOCATIONCONFIG_CLASS_HPP

# include <iostream>
# include <string>

class LocationConfig_class;
# include <ServerConfig.hpp>

# define LIMIT_HTTP_EXCEPT_METH_VAL "limit_except"
# define RESPONSE_RETURN_VAL "return"
# define ROOT_VAL "root"
# define AUTO_INDEX_VAL "autoindex"
# define INDEX_VAL "index"
# define CGIPASS_VAL "fastcgi_pass"
# define METHODS "GET POST DELETE"

struct	Response {
	int	status;
	std::string text; //(optional)
};

class LocationConfig_class
{

	public:

		LocationConfig_class();
		LocationConfig_class( LocationConfig_class const & src );
		~LocationConfig_class();

		LocationConfig_class &		operator=( LocationConfig_class const & rhs );

		void	mapToValues( void );
		void	printValues( void );
		otherVals_map	other_vals;

		void		setPath( std::string value );
		std::string	getPath( void );

	private:
		bool										auto_index;
		std::string									index_file;
		std::string									fastcgi_pass;
		std::string									rootedDir;
		std::vector<std::string>					accepted_methods;
		struct Response								response;
		std::string									path;

		int	redir_validate_fill(otherVals_itc it);
		int	accMeths_validate_fill(otherVals_itc it);
		int	rootedDir_validate_fill(otherVals_itc it);
		int	autoIndex_validate_fill(otherVals_itc it);
		int	fileIndex_validate_fill(otherVals_itc it);
		int	fastCGIpass_validate_fill(otherVals_itc it);

		class AcceptedMethodsException_InvalidMethod : public std::exception {
			public :
				const char* what() const throw(){
					return "accepted_methods: invalid method";
				}
		};
		class ResponseException_InvalidStatus : public std::exception {
			public :
				const char* what() const throw(){
					return "response: invalid status";
				}
		};
		class RootDirException_InvalidRoot : public std::exception {
			public :
				const char* what() const throw(){
					return "root dir: invalid root";
				}
		};
		class AutoIndexException_Error : public std::exception {
			public :
				const char* what() const throw(){
					return "auto index: error";
				}
		};
		class IndexFileException_Error : public std::exception {
			public :
				const char* what() const throw(){
					return "index file: error";
				}
		};
		class CGIpassException_Error : public std::exception {
			public :
				const char* what() const throw(){
					return "index file: error";
				}
		};
		class UnrecognisedCommandException : public std::exception {
			public :
				const char* what() const throw(){
					return "unrecognised command";
				}
		};
};

std::ostream &			operator<<( std::ostream & o, LocationConfig_class const & i );

#endif /* ******************************************** LOCATIONCONFIG_CLASS_H */