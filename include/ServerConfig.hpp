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

// Token types
enum TokenType {
    WORD,
    NUMBER,
    SYMBOL,
    STRING
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
};

static std::string const DEFAULT_CONFIG_PATH = "./data/webserv.default.conf";

std::vector<Token> tokenize(std::string fileConfig);

class ServerConfig
{
	public:
		ServerConfig();
		ServerConfig(std::string const &config_path);
		ServerConfig(ServerConfig const &other);
		ServerConfig const &operator=(ServerConfig const &rhs);
		~ServerConfig();

		bool is_valid() const;

	private:
		bool parse(std::ifstream &file);

		bool	is_valid_;
};

#endif  // WEBSERV_SERVERCONFIG_HPP
