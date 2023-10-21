#include "HttpRequest.hpp"
#include "error.hpp"

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
	if (ParseRequestLine())
		return ;
	PrintRequestLine();
	ParseHeaders();
	std::cout << std::endl;
	PrintHeaders();
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

int	HttpRequest::ParseRequestLine()
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
		return err("invalid request line!", ERROR, -1);
	if (!ParseMeth(reqLineSplit[0]))
		return err("method request invalid!", ERROR, -1);
	_requestTarget = reqLineSplit[1];
	_httpVer = reqLineSplit[2];
	return (0);
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

void	HttpRequest::PrintRequestLine()
{
	std::cout <<
	"Method used: " << _method << std::endl <<
	"Request target: " << _requestTarget << std::endl <<
	"Http version: " << _httpVer << std::endl;
}

int		HttpRequest::ParseHeaders()
{
	if (_rawLines.size() <= 1)
		return err("no headers found in request.", WARNING, 1);
	
	for (size_t i = 1; i < _rawLines.size(); i++)
	{
		size_t	startPos = 0, endPos;
		std::vector<std::string>	keyVal_split;
		endPos = _rawLines[i].find_first_of(HEADER_SEP, startPos);
		if (endPos > startPos)
			keyVal_split.push_back(_rawLines[i].substr(startPos, endPos - startPos));
		startPos = endPos + 1;
		keyVal_split.push_back(_rawLines[i].substr(startPos));
		if (keyVal_split.size() != 2 || endPos == std::string::npos
		|| keyVal_split[1].at(0) != ' ')
		{
			std::cerr << 
			"Header line incorrect at \"" << _rawLines[i] <<
			"\" (Line: " << i+1<<")"<<std::endl;
			continue;
		}
		_headers.insert(std::make_pair(keyVal_split[0], keyVal_split[1].substr(1)));
	}
	return 0;
}

void	HttpRequest::PrintHeaders()
{
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	while (it != _headers.end())
	{
		std::cout <<
		"Key: (" << it->first << "), Value: (" << it->second << ")" << std::endl;
		it++;
	}
	
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */