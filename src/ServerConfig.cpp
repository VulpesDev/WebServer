/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:41:41 by mcutura           #+#    #+#             */
/*   Updated: 2023/11/01 15:41:41 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ServerConfig.hpp>

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---

ServerConfig::ServerConfig() : ServerConfig(DEFAULT_CONFIG_PATH)
{}

ServerConfig::ServerConfig(std::string const &config_path)
{
	std::ifstream	file(config_path);

	if (!file.is_open()) {
		std::cerr << "Cannot open config file: " << config_path << std::endl;
		this->is_valid_ = false;
		return ;
	}
	this->is_valid_ = this->parse(file);
	if (!this->is_valid_) {
		std::cerr << "Invalid configuration file: " << config_path << std::endl;
	}
	file.close();
}

ServerConfig::ServerConfig(ServerConfig const &other)
{}

ServerConfig const &ServerConfig::operator=(ServerConfig const &rhs)
{
	return *this;
}

ServerConfig::~ServerConfig()
{}

////////////////////////////////////////////////////////////////////////////////
// --- GETTERS ---

bool ServerConfig::is_valid() const
{
	return this->is_valid_;
}

////////////////////////////////////////////////////////////////////////////////
// --- INTERNAL ---

bool ServerConfig::parse(std::ifstream &file)
{
	std::string line;

	while (std::getline(file, line)) {
		if (line.empty() || line.compare(0, 1, "#"))
			continue;
		std::cout << line << std::endl;
	}
	return true;
}

bool isNumber(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i]) && str[i] != '-' && str[i] != '+') {
            return false;
        }
    }
    return true;
}

int strChrArr(std::string token, const std::string symbols)
{
    int pos = 0;
    bool found = false;

    for (size_t i = 0; i < symbols.size(); i++) {
        pos = token.find(symbols[i]);
        if (pos != std::string::npos) {
            found = true;
            break;
        }
    }
    return found ? pos : -1;
}

void    tokenizer_core(std::string token, std::vector<Token>& tokens)
{
    Token t;
    int      pos = 0;
    const std::string symbols = "{};=,";

    if (token[0] == '"' || token[0] == '\'') {
        t.type = STRING;
        t.value = token.substr(1, token.length() - 2); // Remove quotes
    } else if (isNumber(token)) {
        t.type = NUMBER;
        t.value = token;
    } else if ((pos = strChrArr(token, symbols)) >= 0) {
        if (pos == 0) {
            t.type = SYMBOL;
            t.value = token;
        } else {
            tokenizer_core(token.substr(0, pos), tokens);
            tokenizer_core(token.substr(pos), tokens);
            return;
        }
    } else {
        t.type = WORD;
        t.value = token;
    }

    tokens.push_back(t);
}

std::vector<Token> tokenizeLine(const std::string& line) {
    std::vector<Token> tokens;
    std::stringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokenizer_core(token, tokens);
    }

    return tokens;
}
