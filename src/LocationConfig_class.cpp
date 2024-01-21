#include "LocationConfig_class.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

LocationConfig_class::LocationConfig_class()
{
}

LocationConfig_class::LocationConfig_class( const LocationConfig_class & src )
{
	*this = src;
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
	if ( this != &rhs ) {
		path = rhs.path;
		response = rhs.response;
		rootedDir = rhs.rootedDir;
		auto_index = rhs.auto_index;
		index_file = rhs.index_file;
		fastcgi_pass = rhs.fastcgi_pass;
		for (std::vector<std::string>::const_iterator i = rhs.accepted_methods.begin(); i != rhs.accepted_methods.end(); i++)
			accepted_methods.push_back(*i);
		for (otherVals_itc i = rhs.other_vals.begin(); i != rhs.other_vals.end(); i++)
			this->other_vals.insert(*i);
	}
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

bool isNumericLoc(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return false;
        }
    }
    return true;
}

int	LocationConfig_class::accMeths_validate_fill(otherVals_itc it) {
	if (it->first == LIMIT_HTTP_EXCEPT_METH_VAL) {
		for (std::vector<std::string>::const_iterator i = it->second.begin(); i != it->second.end(); i++) {
			if (!isMethod(*i))
				throw AcceptedMethodsException_InvalidMethod();
			accepted_methods.push_back(*i);
		}
		return (1);
	}
	return (0);
}

int LocationConfig_class::redir_validate_fill(otherVals_itc it) {
	if (it->first == RESPONSE_RETURN_VAL) {
		if (!it->second.at(0).empty()) {
			if (!isNumericLoc(it->second.at(0))) {
				throw ResponseException_InvalidStatus();
			}
			response.status = std::atoi(it->second.at(0).c_str());
		}
		else
			throw ResponseException_InvalidStatus();
		if (!it->second.at(1).empty())
			response.text = it->second.at(1);
		return (1);
	}
	return (0);
}

int LocationConfig_class::rootedDir_validate_fill(otherVals_itc it) {
	if (it->first == ROOT_VAL) {
		if (!it->second.at(0).empty())
			rootedDir = it->second.at(0);
		else
			throw RootDirException_InvalidRoot();
		return (1);
	}
	return (0);
}

int LocationConfig_class::autoIndex_validate_fill(otherVals_itc it) {
	if (it->first == AUTO_INDEX_VAL) {
		if (!it->second.at(0).empty() && it->second.at(0) == "0")
			auto_index = 0;
		else if (!it->second.at(0).empty() && it->second.at(0) == "1")
			auto_index = 1;
		else
			throw AutoIndexException_Error();
		return (1);
	}
	return (0);
}

int LocationConfig_class::fileIndex_validate_fill(otherVals_itc it) {
	if (it->first == INDEX_VAL) {
		if (!it->second.at(0).empty())
			index_file = it->second.at(0);
		else
			throw IndexFileException_Error();
		return (1);
	}
	return (0);
}

int LocationConfig_class::fastCGIpass_validate_fill(otherVals_itc it) {
	if (it->first == CGIPASS_VAL) {
		if (!it->second.at(0).empty())
			fastcgi_pass = it->second.at(0);
		else
			throw CGIpassException_Error();
		return (1);
	}
	return (0);
}

void	LocationConfig_class::mapToValues(void) {
	for (otherVals_itc it = other_vals.begin(); it != other_vals.end(); it++) {
		try {
			if (accMeths_validate_fill(it) || redir_validate_fill(it) ||
				rootedDir_validate_fill(it) || autoIndex_validate_fill(it) ||
				fileIndex_validate_fill(it) || fastCGIpass_validate_fill(it))
				{;}
			else
				throw UnrecognisedCommandException();
		} catch(const std::exception& e) {
			std::cerr << "\033[1;31m" <<  "Error: " << e.what() << "\033[0m" << '\n';
		}
	}
}

void	LocationConfig_class::printValues(void) {
	std::cout << "Limit except: ";
	for (std::vector<std::string>::iterator i = accepted_methods.begin();
	i != accepted_methods.end(); i++) {
		std::cout << *i << "-";
	}
	std::cout << std::endl;

	std::cout << "Response: " << std::endl;
	std::cout << " -status: " << response.status << std::endl;
	std::cout << " -text: " << response.text << std::endl;

	std::cout << "Root: " << rootedDir << std::endl;
	std::cout << "Auto index: " << auto_index << std::endl;

	std::cout << "Index: " << index_file << std::endl;

	std::cout << "CGI: " << fastcgi_pass << std::endl;
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void		LocationConfig_class::setPath( std::string value ) {
	path = value;
}

std::string		LocationConfig_class::getPath( void ) {
	return path;
}

/* ************************************************************************** */