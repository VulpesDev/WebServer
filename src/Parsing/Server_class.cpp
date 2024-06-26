#include "Server_class.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server() {
	server_name.clear();
	err_pages.clear();
	other_vals.clear();
	locations.clear();
	server_name.push_back(DEFAULT_SERVERNAME);
	max_body_size = DEFAULT_BODYSIZE;
	port = DEFAULT_PORT;
	//sdhp_set = false;
}
	
Server::Server( const Server & src ) {
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{	
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs ) {
		this->fd = rhs.fd;
		this->server_name = rhs.server_name;
		this->max_body_size = rhs.max_body_size;
		this->port = rhs.port;

		for (errPages_itc itc = rhs.err_pages.begin(); itc != rhs.err_pages.end(); itc++) {
			ErrorPage ep;
			for (std::vector<int>::const_iterator itc2 = itc->errs.begin(); itc2 != itc->errs.end(); itc2++) {
				ep.errs.push_back(*itc2);
			}
			ep.path = itc->path;
			this->err_pages.push_back(ep);
		}

		for (otherVals_itc itc = rhs.other_vals.begin(); itc != rhs.other_vals.end(); itc++) {
			this->other_vals.insert(*itc);
		}
		for (std::vector<Location>::const_iterator itc = rhs.locations.begin(); itc != rhs.locations.end(); itc++) {
			this->locations.push_back(*itc);
		}
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Server const & i )
{
	//o << "Value = " << i.getValue();
	o << "Server Fd: " << i.getFd() << std::endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

/// @brief Checks wether a file exists and if the user has access to it
/// @param filePath string representing the path of the file
/// @return true or false logically
bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath.c_str());
    return file.good();
}

bool isOverflow(const std::string s) {
	long l;

	l = 0;
	for (std::string::const_iterator itc = s.begin(); itc != s.end() && std::isdigit(*itc); itc++) {
		l = l * 10 + (*itc - '0');
	}
	if (s.length() > 10 || l > 2147483647 || l < -2147483648) {
		return true;
	}
	return false;
}

/// @brief Checks wether a string is numeric or not
/// @param str The string to be checked
/// @return true or false logically
bool isNumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return false;
        }
    }
    return true;
}

/// @brief This function validates and populates the server_name member of the class
/// @param it Constant iterator of othervals map
/// @return 0 on success or Err_ServerName on error
int	Server::servName_validate_fill(otherVals_itc it)
{
	if (it->first == SERV_NAME_VAL) {
		if (it->second.empty())			//check if the it->second is not empty
			throw ServerName_Exception();		//error then I'd also have to log it somehow
		server_name = it->second;
		return 1;
	}
	return 0;
}

/// @brief This function validates and populates the max_body_size member of the class
/// @param it Constant iterator of othervals map
/// @return Warn_None, Err_None on success or Err_BodySize_Unit, Err_BodySize_Numval or Warn_BodySize_Missing
int	Server::maxBodySize_validate_fill(otherVals_itc it)
{
	if (it->first == CLIENT_BODY_SIZE_VAL) {
		if (it->second.size() <= 0)
			throw BodySize_Exception();
		std::string	val = it->second.at(0);							//possibly not existing
		if (val.empty())
			throw BodySize_Exception();
		if (isOverflow(it->second.at(0)))
			throw NumberOverflowException();
		int			numVal;
		std::stringstream	ss(it->second.at(0));
		if (!(ss >> numVal))
			throw BodySizeNnumval_Exception();
		char	c = val.at(val.length() - 1);

		if (numVal <= 0)
			throw BodySizeNnumval_Exception();
		switch (c) {
		case 'K':
			numVal *= K;
			break;
		case 'M':
			numVal *= M;
			break;
		case 'G':
			numVal *= G;
			break;
		default:
			throw BodySizeUnit_Exception();
		}
		max_body_size = numVal;
		return (1);
	}
	return (0);
}

/// @brief This function validates and populates the port member of the class
/// @param it Constant iterator of othervals map
/// @return Warn_None, Err_None on success or Err_Port_WrongParam and Warn_Port_Missing
int	Server::host_port_validate_fill(otherVals_itc it)
{
	if (it->first == PORT_VAL) {
		if (it->second.size() <= 0 || it->second.at(0).empty())
			throw PortWrongParam_Exception();
		if (isOverflow(it->second.at(0)))
			throw NumberOverflowException();
		if (isOverflow(it->second.at(0)))
			throw NumberOverflowException();
		std::string const &val = it->second.at(0);
		if (val.empty())
			throw PortWrongParam_Exception();
		int	result = 0;
		std::string::const_iterator	itr = val.begin();
		while (itr != val.end() && isdigit(*itr)) {
			result *= 10;
			result += *itr - '0';
			++itr;
		}
		if (itr != val.end())
			throw PortWrongParam_Exception();
		if (result > 0 && result < MaxPortNum) {
			std::stringstream stream;
			stream << result;
			port = stream.str();
		}
		else
			throw PortWrongParam_Exception();//error
		return 1;
	}
	return 0;
}

/// @brief This function validates and populates the err_pages member of the class
/// @param it Constant iterator of othervals map
/// @return Err_None, Warn_None on success or Err_ErrPage_File and Warn_ErrPage_Missing
int	Server::errorPages_validate_fill(otherVals_itc it) {
	int					error;
	ErrorPage			page;

	if (it->first == ERR_PAGE_VAL) {
		page.path = it->second.back();					// check if its null
		if (page.path.empty() || !fileExists(page.path))
			throw ErrorPageFile_Exception();					//log error

		for (std::vector<std::string>::const_iterator i = it->second.begin();
		i != it->second.end(); i++) {
			if (*i == it->second.back())
				continue;
			if (!isNumeric(*i))
				throw ErrorPageNotNumericException();
			if (isOverflow(*i))
				throw NumberOverflowException();
			error = 0;
			std::string::const_iterator it = i->begin();
			while (it != i->end() && *it >= '0' && *it <= '9') {
				error *= 10;
				error += *it - '0';
				++it;
			}
			if (error <= 0)
				throw ErrorPageErrorException();
			page.errs.push_back(error);
		}
		err_pages.push_back(page);
		return 1;
	}
	return 0;
}

/// @brief Iterates through otherVals map and assigns fills in the member
///		variables of the class accordingly (also validating the integrity
///		and correctness of the data)
/// @param  
void	Server::mapToValues( void ) {
	for (otherVals_itc it = other_vals.begin(); it != other_vals.end(); it++) {
		try {
			if (servName_validate_fill(it) || maxBodySize_validate_fill(it) ||
				host_port_validate_fill(it) || errorPages_validate_fill(it))
				{;}
			else
				throw UnrecognisedCommandException();
		} catch(const std::exception& e) {
			std::cerr << "\033[1;31m" << "Error: " << e.what() << "\033[0m" << '\n';
		}
	}
}

/// @brief Prints all the member values of the class
/// @param  
void	Server::printValues( void ) const{

	std::cout << "Server name: ";
	for(std::vector<std::string>::const_iterator it = server_name.begin();
	    it != server_name.end(); it++) {
		std::cout << *it << " ";
	} std::cout << std::endl;

	std::cout << "Port: " << port << std::endl;

	std::cout << "Max bodysize: " << max_body_size << std::endl;

	std::cout << "Error pages: ";
	for (std::vector<ErrorPage>::const_iterator epIt = err_pages.begin();
	     epIt != err_pages.end(); epIt++)
	{
		for (std::vector<int>::const_iterator errIt = (*epIt).errs.begin();
		     errIt != (*epIt).errs.end(); errIt++)
		{
			std::cout << *errIt << " ";
		}
		std::cout << (*epIt).path << std::endl;
	}

	std::cout << "Locations: " << std::endl;
	for (std::vector<Location>::const_iterator locIt = locations.begin();
	     locIt != locations.end(); locIt++) {
		(*locIt).printValues();
	}
	std::cout << std::endl;

}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string					Server::GetPort() const {
	return this->port;
}
unsigned long				Server::GetMaxBodySize() const {
	return this->max_body_size;
}
// bool						Server::GetDhpSet() {
// 	return this->dhp_set;
// }
errPages_arr				Server::GetErrPages() const {
	return this->err_pages;
}
std::vector<std::string>	Server::GetServNames() const {
	return this->server_name;
}
// std::pair<std::string, int>	Server::GetDefHostPort() {
// 	return this->default_host_port;
// }

/* ************************************************************************** */
