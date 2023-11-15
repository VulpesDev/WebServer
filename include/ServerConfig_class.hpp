#ifndef SERVERCONFIG_CLASS_HPP
# define SERVERCONFIG_CLASS_HPP

# include <iostream>
# include <string>
# include <ServerConfig.hpp>

class ServerConfig_class
{

	public:

		ServerConfig_class();
		ServerConfig_class( ServerConfig_class const & src );
		~ServerConfig_class();

		ServerConfig_class &		operator=( ServerConfig_class const & rhs );

		void	printValues( void );

	private:
		static bool 						dhp_set;
		static std::pair<std::string, int>	default_host_port;
		std::vector<std::string>			server_name;
		int									max_body_size;
		int									port;
		errPages_arr						err_pages;
		otherVals_map						other_vals;
		std::vector<Location>				locations;
		
		void	mapToValues( void );
		int		errorPages_validate_fill(otherVals_itc it);
		int		host_port_validate_fill(otherVals_itc it);
		int		maxBodySize_validate_fill(otherVals_itc it);
		int		servName_validate_fill(otherVals_itc it);

};

std::ostream &			operator<<( std::ostream & o, ServerConfig_class const & i );

#endif /* ********************************************** SERVERCONFIG_CLASS_H */