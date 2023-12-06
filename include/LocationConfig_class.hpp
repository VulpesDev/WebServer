#ifndef LOCATIONCONFIG_CLASS_HPP
# define LOCATIONCONFIG_CLASS_HPP

# include <iostream>
# include <string>

class LocationConfig_class;
# include <ServerConfig.hpp>


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
		std::string	path;
		std::string	root;

};

std::ostream &			operator<<( std::ostream & o, LocationConfig_class const & i );

#endif /* ******************************************** LOCATIONCONFIG_CLASS_H */