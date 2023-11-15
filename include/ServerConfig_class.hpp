#ifndef SERVERCONFIG_CLASS_HPP
# define SERVERCONFIG_CLASS_HPP

# include <iostream>
# include <string>

class ServerConfig_class;
# include <ServerConfig.hpp>

# define DEFAULT_PORT 80
# define DEFAULT_BODYSIZE 0

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
		std::vector<Location>				locations;

	private:
		static bool 						dhp_set;
		static std::pair<std::string, int>	default_host_port;
		std::vector<std::string>			server_name;
		int									max_body_size;
		int									port;
		errPages_arr						err_pages;
		
		
		
		int		errorPages_validate_fill(otherVals_itc it);
		int		host_port_validate_fill(otherVals_itc it);
		int		maxBodySize_validate_fill(otherVals_itc it);
		int		servName_validate_fill(otherVals_itc it);

};

std::ostream &			operator<<( std::ostream & o, ServerConfig_class const & i );

#endif /* ********************************************** SERVERCONFIG_CLASS_H */