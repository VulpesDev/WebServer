/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 16:51:41 by mcutura           #+#    #+#             */
/*   Updated: 2024/02/14 22:48:20 by tvasilev         ###   ########.fr       */
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

#include <stack>
#include <unordered_map>

# define SYMBOLS "{};=,#"
# define KEYWORDS "http server location"

# define K 1
# define M 1*1000
# define G 1*1000*1000

# define MaxPortNum 65535 //Suggested by chatGPT


struct ErrorPage {
	std::string			path; //the path to the page
	std::vector<int>	errs; //stores the errors as int vars
};

typedef	std::vector<ErrorPage>					errPages_arr;
typedef	std::vector<ErrorPage>::iterator		errPages_it;
typedef	std::vector<ErrorPage>::const_iterator	errPages_itc;

typedef std::multimap<std::string, std::vector<std::string> > otherVals_map;
typedef std::multimap<std::string, std::vector<std::string> >::iterator otherVals_it;
typedef std::multimap<std::string, std::vector<std::string> >::const_iterator otherVals_itc;

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

# include <Location_class.hpp>
# include <Server_class.hpp>

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

		std::vector<Http> 	GetHttps( void ) const;
		Server 				GetFirstServer( void );

	private:

		std::vector<Token>	tokens;
		std::vector<Server>	servers;
		std::vector<Http>	https;

		bool				is_valid_;
		bool				parse(std::ifstream& file);

		int					mapToStruct(Http& h);

		bool				isValidBraces(std::vector<Token> tokens);
		bool				isValidSemicolon(std::vector<Token> tokens);
		bool				isValidEncapsulation(std::vector<Token> tokens);
};

#endif  // WEBSERV_SERVERCONFIG_HPP
