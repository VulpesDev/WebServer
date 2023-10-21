#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <cstring>
# include <map>
# include <vector>
# define SPACES " \t\v\f"
# define NEW_LINES "\r\n"
# define HEADER_SEP ":"

class HttpRequest
{

	public:

		HttpRequest(std::string raw);
		HttpRequest( HttpRequest const & src );
		~HttpRequest();

		HttpRequest &		operator=( HttpRequest const & rhs );

	private:
		enum	_methods
		{
			GET,
			PUT,
			POST,
			HEAD,
			OPTIONS
		};
		HttpRequest::_methods	_method;
		std::string	_rawRequest;
		std::vector<std::string>	_rawLines;
		std::string	_requestTarget;
		std::string _httpVer;

		std::map<std::string, std::string>	_headers;
		std::map<std::string, std::string>	_requestHeaders;
		std::map<std::string, std::string>	_generalHeaders;
		std::map<std::string, std::string>	_representHeaders;
		std::string	_body;

		int		ParseRequestLine();
		void	PrintRequestLine();
		int		ParseHeaders();
		void	PrintHeaders();
		int		ParseMeth(std::string str);
};

std::ostream &			operator<<( std::ostream & o, HttpRequest const & i );

#endif /* ***************************************************** HTTPREQUEST_H */