#ifndef LOCATIONCONFIG_CLASS_HPP
# define LOCATIONCONFIG_CLASS_HPP

# include <iostream>
# include <string>

class LocationConfig_class;
# include <ServerConfig.hpp>

# define LIMIT_HTTP_EXCEPT_METH_VAL "limit_except"
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

	private:
		bool										auto_index;
		std::string									path;
		std::string									alias;
		std::string									index_file;
		std::string									fastcgi_pass;
		std::vector<std::string>					accepted_methods;
		struct Response								response;

		int	accMeths_validate_fill(otherVals_itc it);
		int redir_validate_fill(otherVals_itc it);

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
};

std::ostream &			operator<<( std::ostream & o, LocationConfig_class const & i );

#endif /* ******************************************** LOCATIONCONFIG_CLASS_H */