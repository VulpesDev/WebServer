#ifndef LOCATION_CLASS_HPP
# define LOCATION_CLASS_HPP

# include <iostream>
# include <string>

class Location;
# include <ServerConfig.hpp>

# define ROOT_VAL "root"
# define INDEX_VAL "index"
# define METHODS "GET POST DELETE"
# define AUTO_INDEX_VAL "autoindex"
# define CGIPASS_VAL "fastcgi_pass"
# define RESPONSE_RETURN_VAL "return"
# define LIMIT_HTTP_EXCEPT_METH_VAL "limit_except"

struct	Response {
	int	status;
	std::string text; //(optional)
};

class Location
{
	private:
		std::string									path;
		struct Response								response;
		std::string									rootedDir;
		std::string									index_file;
		bool										auto_index;
		std::string									fastcgi_pass;
		std::vector<std::string>					accepted_methods;

		int	redir_validate_fill(otherVals_itc it);
		int	accMeths_validate_fill(otherVals_itc it);
		int	rootedDir_validate_fill(otherVals_itc it);
		int	autoIndex_validate_fill(otherVals_itc it);
		int	fileIndex_validate_fill(otherVals_itc it);
		int	fastCGIpass_validate_fill(otherVals_itc it);
		
	public:

		Location();
		Location( Location const & src );
		~Location();

		Location &		operator=( Location const & rhs );

		void	mapToValues( void );
		void	printValues( void ) const;
		otherVals_map	other_vals;

		std::string	getPath( void );
		void		setPath( std::string value );


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
		class NumberOverflowException : public std::exception {
			public :
				const char* what() const throw(){
					return "number is either too big or too small";
				}
		};

		////// GETTERS
	std::string getPath() const {
        return path;
    }
    
    Response getResponse() const {
        return response;
    }
    
    std::string getRootedDir() const {
        return rootedDir;
    }
    
    std::string getIndexFile() const {
        return index_file;
    }
    
    bool getAutoIndex() const {
        return auto_index;
    }
    
    std::string getFastcgiPass() const {
        return fastcgi_pass;
    }
    
    std::vector<std::string> getAcceptedMethods() const {
        return accepted_methods;
    }
};

std::ostream &			operator<<( std::ostream & o, Location const & i );

#endif /* ******************************************** Location_H */