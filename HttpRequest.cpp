#include "HttpRequest.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HttpRequest::HttpRequest(std::string raw) : _rawRequest(raw)
{
	size_t	startPos = 0, endPos;
	while ((endPos = _rawRequest.find_first_of(NEW_LINES, startPos)) != std::string::npos)
	{
		if (endPos > startPos)
			_rawLines.push_back(_rawRequest.substr(startPos, endPos - startPos));
		startPos = endPos + 1;
	}
	ParseRequestLine();
}

HttpRequest::HttpRequest( const HttpRequest & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HttpRequest::~HttpRequest()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

HttpRequest &				HttpRequest::operator=( HttpRequest const & rhs )
{
	this->_rawRequest = rhs._rawRequest;
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

// std::ostream &			operator<<( std::ostream & o, HttpRequest const & i )
// {
// 	//o << "Value = " << i.getValue();
// 	//o << i << std::endl;
// 	return o;
// }


/*
** --------------------------------- METHODS ----------------------------------
*/

void	HttpRequest::ParseRequestLine()
{
	std::string requestLine = _rawLines[0];
	std::vector<std::string>	reqLineSplit;
	size_t	startPos = 0, endPos;

	while ((endPos = requestLine.find_first_of(SPACES, startPos)) != std::string::npos)
	{
		if (endPos > startPos)
			reqLineSplit.push_back(requestLine.substr(startPos, endPos - startPos));
		startPos = endPos + 1;
	}
	if (startPos < requestLine.length())
	    reqLineSplit.push_back(requestLine.substr(startPos));
	if (reqLineSplit.size() != 3)
	{
		std::cerr << "Error: invalid request line!" << std::endl;
		return;
	}
	if (!ParseMeth(reqLineSplit[0]))
		std::cerr << "Error: method request invalid!" << std::endl;
	_requestTarget = reqLineSplit[1];
	_httpVer = reqLineSplit[2];
	std::cout <<
	"Method used: " << _method << std::endl <<
	"Request target: " << _requestTarget << std::endl <<
	"Http version: " << _httpVer << std::endl;
}

int		HttpRequest::ParseMeth(std::string str)
{
	if (str == "GET")
		_method = GET;
	else if (str == "PUT")
		_method = PUT;
	else if (str == "POST")
		_method = POST;
	else if (str == "HEAD")
		_method = HEAD;
	else if (str == "OPTIONS")
		_method = OPTIONS;
	else
		return (0);
	return (1);
	
	
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */