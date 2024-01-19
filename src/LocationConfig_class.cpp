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

bool isMethod(std::string s) {
    const std::string keywords = METHODS;
    return keywords.find(s) == std::string::npos ? 0 : 1;
}

bool isNumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return false;
        }
    }
    return true;
}

int	LocationConfig_class::accMeths_validate_fill(otherVals_itc it) {
	if (it->first == "limit_except") {
		for (std::vector<std::string>::const_iterator i = it->second.begin();
		 i < it->second.end(); i++) {
			if (!isMethod(*i))
				throw AcceptedMethodsException_InvalidMethod();
			accepted_methods.push_back(*i);
		}
		
	}
}

int LocationConfig_class::redir_validate_fill(otherVals_itc it) {
	if (it->first == "return") {
		if (!it->second.at(0).empty()) {
			if (!isNumeric(it->second.at(0))) {
				throw ResponseException_InvalidStatus();
			}
			response.status = std::atoi(it->second.at(0).c_str());
		}
		else
			throw ResponseException_InvalidStatus();
		if (!it->second.at(1).empty())
			response.text = it->second.at(1);
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */