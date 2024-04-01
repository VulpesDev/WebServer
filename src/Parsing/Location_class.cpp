#include "Location_class.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location()
{
		path = "/";
		redir.status = 0;
		redir.text = "";
		rootedDir = "data/www";
		index_file = "";
		auto_index = false;
		fastcgi_pass = "";
		accepted_methods.push_back("GET");
		accepted_methods.push_back("POST");
}

Location::Location( const Location & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Location::~Location()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Location &				Location::operator=( Location const & rhs )
{
	if ( this != &rhs ) {
		path = rhs.path;
		redir = rhs.redir;
		redir.status = rhs.redir.status;
		redir.text = rhs.redir.text;
		rootedDir = rhs.rootedDir;
		auto_index = rhs.auto_index;
		index_file = rhs.index_file;
		fastcgi_pass = rhs.fastcgi_pass;
		for (std::vector<std::string>::const_iterator i = rhs.accepted_methods.begin(); i != rhs.accepted_methods.end(); i++){
			accepted_methods.push_back(*i);
		}
		for (otherVals_itc i = rhs.other_vals.begin(); i != rhs.other_vals.end(); i++)
			this->other_vals.insert(*i);
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Location const & i )
{
	//o << "Value = " << i.getValue();
	o << "path = " << i.getPath() << std::endl;
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

bool isOverflowl(const std::string s) {
	long l = 0;
	for (std::string::const_iterator itc = s.begin(); itc != s.end() && std::isdigit(*itc); itc++) {
		l = l * 10 + (*itc - '0');
	}
	if (s.length() > 10 || l > 2147483647 || l < -2147483648) {
		return true;
	}
	return false;
}

int	Location::accMeths_validate_fill(otherVals_itc it) {
	accepted_methods.clear();
	if (it->first == LIMIT_HTTP_EXCEPT_METH_VAL) {
		for (std::vector<std::string>::const_iterator i = it->second.begin(); i != it->second.end(); i++) {
			if (!isMethod(*i))
				throw AcceptedMethodsException_InvalidMethod();
			std::cerr << "Accepted method: " << *i << std::endl;
			accepted_methods.push_back(*i);
		}
		return (1);
	}
	return (0);
}

int Location::redir_validate_fill(otherVals_itc it) {
	if (it->first == REDIRECTION_RETURN_VAL) {
		if (!it->second.at(0).empty()) {
			if (!isNumericLoc(it->second.at(0))) {
				throw RedirectionException_InvalidStatus();
			}
			if (isOverflowl(it->second.at(0))) {
				throw NumberOverflowException();
			}
			std::istringstream iss(it->second.at(0));
			iss >> redir.status;
		}
		else
			throw RedirectionException_InvalidStatus();
		if (!it->second.at(1).empty())
			redir.text = it->second.at(1);
		return (1);
	}
	return (0);
}

int Location::rootedDir_validate_fill(otherVals_itc it) {
	if (it->first == ROOT_VAL) {
		if (!it->second.at(0).empty())
			rootedDir = it->second.at(0);
		else
			throw RootDirException_InvalidRoot();
		return (1);
	}
	return (0);
}

int Location::autoIndex_validate_fill(otherVals_itc it) {
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

int Location::fileIndex_validate_fill(otherVals_itc it) {
	if (it->first == INDEX_VAL) {
		if (!it->second.at(0).empty())
			index_file = it->second.at(0);
		else
			throw IndexFileException_Error();
		return (1);
	}
	return (0);
}

int Location::fastCGIpass_validate_fill(otherVals_itc it) {
	if (it->first == CGIPASS_VAL) {
		if (!it->second.at(0).empty())
			fastcgi_pass = it->second.at(0);
		else
			throw CGIpassException_Error();
		return (1);
	}
	return (0);
}

void	Location::mapToValues(void) {
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


//only printing values from one server.
void	Location::printValues(void) const {
	
	std::cout << "Path: " << path << std::endl;
	std::cout << "Limit except: ";
	for (std::vector<std::string>::const_iterator i = accepted_methods.begin();
	i != accepted_methods.end(); i++) {
		std::cout << *i << "-";
	}
	std::cout << std::endl;

	std::cout << "Redirection: " << std::endl;
	std::cout << " -status: " << redir.status << std::endl;
	std::cout << " -text: " << redir.text << std::endl;

	std::cout << "Root: " << rootedDir << std::endl;
	std::cout << "Auto index: " << auto_index << std::endl;

	std::cout << "Index: " << index_file << std::endl;

	std::cout << "CGI: " << fastcgi_pass << std::endl;
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void		Location::setPath( std::string value ) {
	path = value;
}

std::string		Location::getPath( void ) {
	return path;
}

/* ************************************************************************** */
