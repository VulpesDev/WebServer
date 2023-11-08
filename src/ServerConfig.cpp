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

bool				ServerConfig::parse(std::ifstream& file)
{
    tokens = tokenize(file);
    for (std::vector<Token>::iterator token = tokens.begin(); token != tokens.end(); token++) {
        if (token->type == KEYWORD && token->value == "http" && ++token != tokens.end()){
            if (token->type != SYMBOL || token->value != "{") {
                    std::cerr << "Expected \'{\' at line " << token->line << std::endl;
                    return false;
                }
            while (token->value != "}" && ++token != tokens.end()) {
                if (token->type == KEYWORD && token->value == "server" && ++token != tokens.end()){
                    Server server;
                    if (token->value != "{") {
                        std::cerr << "Expected \'{\' at line " << token->line << std::endl;
                        return false;
                    }
                    while (token->value != "}" && ++token != tokens.end()) {
                        //Parse server
                        std::cout << "Parsing server!" << std::endl;
                    }
                    if (token->value != "}"){
                        std::cerr << "Expected \'}\' at end of block" << std::endl;
                        return false;
                    }else {
                        ++token;
                        if (token == tokens.end()){
                            std::cerr << "Expected \'}\' at end of block" << std::endl;
                            return false;
                        }
                    }
                    servers.push_back(server);
                } else if (token->type == KEYWORD && token->value == "location") {
                    Location location;
                    //Parse location
                    std::cout << "Parsing location!" << std::endl;
                    servers.back().locations.push_back(location);
                } else if (token->type == KEYWORD || token->type == WORD || token->type == NUMBER || token->type == SYMBOL){
                    continue;
                } else if (token->type == SYMBOL && token->value == "#")
                    continue;
                else {
                    std::cerr << "(server)Unexpected \"" << token->value << "\" at line " << token->line << std::endl;
                    return false;
                }
            }
        } else if (token->type == KEYWORD || token->type == WORD || token->type == SYMBOL){
            continue;
        } else if (token->type == SYMBOL && token->value == "#")
            continue;
        else {
            std::cerr << "Unexpected \"" << token->value << "\" at line " << token->line << std::endl;
            return false;
        }
    } return true;
}
