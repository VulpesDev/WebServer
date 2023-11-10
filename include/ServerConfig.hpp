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

# define SYMBOLS "{};=,#"
# define KEYWORDS "http server location"

enum TokenType {
	KEYWORD,
    WORD,
    NUMBER,
    SYMBOL,
    STRING
};

struct Token {
    TokenType type;
    std::string value;
	unsigned int	line;
};

struct Location {
	std::string path;
	std::string root;
};

struct Server {
	std::string server_name;
	std::string host;
	int port;
	std::vector<Location> locations;
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
		bool				is_valid_;
		bool				parse(std::ifstream& file);
		bool				isValidBraces(std::vector<Token> tokens);
		bool				isValidSemicolon(std::vector<Token> tokens);
		bool				isValidEncapsulation(std::vector<Token> tokens);
		std::vector<Token>	tokens;
		std::vector<Server>	servers;
};

#endif  // WEBSERV_SERVERCONFIG_HPP
