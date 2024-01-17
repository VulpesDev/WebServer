#include "LocationConfig_class.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LocationConfig_class::LocationConfig_class()
{
}

LocationConfig_class::LocationConfig_class( const LocationConfig_class & src )
{
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

LocationConfig_class::~LocationConfig_class()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

LocationConfig_class &				LocationConfig_class::operator=( LocationConfig_class const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, LocationConfig_class const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */