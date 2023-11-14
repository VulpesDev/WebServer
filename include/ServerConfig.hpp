/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 16:51:41 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/28 16:51:41 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVERCONFIG_HPP
# define WEBSERV_SERVERCONFIG_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <sstream>
# include <map>

# define SYMBOLS "{};=,#"
# define KEYWORDS "http server location"

# define K 1000
# define M 1000*1000
# define G 1000*1000*1000

# define MaxPortNum 65535 //Suggested by chatGPT

typedef	std::vector<ErrorPage>					errPages_arr;
typedef	std::vector<ErrorPage>::iterator		errPages_it;
typedef	std::vector<ErrorPage>::const_iterator	errPages_itc;


typedef std::multimap<std::string, std::vector<std::string>> otherVals_map;
typedef std::multimap<std::string, std::vector<std::string>>::iterator otherVals_it;
typedef std::multimap<std::string, std::vector<std::string>>::const_iterator otherVals_itc;

enum WarningCodes {
	Warn_None,
	Warn_ServerName_Missing,
	Warn_BodySize_Missing,
	Warn_Port_Missing,
	Warn_ErrPage_Missing
};
enum ErrorCodes {
	Err_None,
	Err_ServerName,
	Err_BodySize,
	Err_BodySize_Unit,
	Err_BodySize_Numval,
	Err_Port_WrongParam,
	Err_ErrPage_File,
	Err_ErrPage_NotNumeric,
	Err_ErrPage_LessZero

};

enum TokenType {
	KEYWORD,
    WORD,
    NUMBER,
    SYMBOL,
    STRING
};

struct Token {
    TokenType		type;
    std::string		value;
	unsigned int	line;
};

struct Location {
	std::string		path;
	std::string		root;
	otherVals_map	other_vals;
};

struct ErrorPage {
	std::string			path; //the path to the page
	std::vector<int>	errs; //stores the errors as int vars
};

struct Server {
	static bool dhp_set;
	static std::pair<std::string, int>	default_host_port;
	std::vector<std::string>			server_name;
	int									max_body_size;
	int									port;
	errPages_arr						err_pages;
	otherVals_map						other_vals;
	std::vector<Location>				locations;
};

struct Http {
	std::vector<Server>	servers;
	otherVals_map		other_vals;
};

static std::string const DEFAULT_CONFIG_PATH = "./data/webserv.default.conf";

class ServerConfig
{
	public:
		ServerConfig();
		ServerConfig(std::string const &config_path);
		ServerConfig(ServerConfig const &other);
		ServerConfig const &operator=(ServerConfig const &rhs);
		~ServerConfig();

		bool is_valid() const;
		std::vector<Token> tokenize(std::ifstream& file);

	private:

		std::vector<Token>	tokens;
		std::vector<Server>	servers;
		std::vector<Http>	https;

		bool				is_valid_;
		bool				parse(std::ifstream& file);

		int					mapToStruct(Http& h);
		int					mapToStruct(Server& s);
		int					mapToStruct(Location& l);

		bool				isValidBraces(std::vector<Token> tokens);
		bool				isValidSemicolon(std::vector<Token> tokens);
		bool				isValidEncapsulation(std::vector<Token> tokens);
};

#endif  // WEBSERV_SERVERCONFIG_HPP
