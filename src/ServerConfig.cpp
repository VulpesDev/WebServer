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
		file.close();
		return ;
	}
	file.close();
	// for (std::vector<Http>::iterator it = https.begin(); it != https.end(); it++) {
	// 	for (std::vector<Server>::iterator i = it->servers.begin(); i != it->servers.end(); i++) {
	// 		mapToStruct(*i);
	// 	}
	// }
	
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

Location				parseLocations(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Location l;
	it+=1;
	l.path = it->value;
	it += 2;
	while ((it != tokens.end() && (it->type != SYMBOL && it->value != "}")) && (it->type == WORD || it->type == NUMBER)) {
		std::string					val_key;
		std::vector<std::string>	val_values;
		
		val_key = it->value;
		it++;
		while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
			val_values.push_back(it->value);
			it++;
		}
		if (!val_key.empty())
		l.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
		it++;
	} return l;
}

ServerConfig_class				parseServer(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	ServerConfig_class s;
	it+=2;
	while (it != tokens.end() && (it->type != SYMBOL && it->value != "}")) {
		if (it->type == KEYWORD && it->value == "location") {
			s.locations.push_back(parseLocations(it, tokens));
		} else if (it->type == WORD) {
			std::string					val_key;
			std::vector<std::string>	val_values;
			
			val_key = it->value;
			it++;
			while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
				val_values.push_back(it->value);
				it++;
			}
			if (!val_key.empty()) {
				s.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
			}
		} it++;
	} return s;
}


Http				parseHttp(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Http h;
	it+=2;
	while (it != tokens.end() && (it->type != SYMBOL && it->value != "}")) {
		if (it->type == KEYWORD && it->value == "server") {
			h.servers.push_back(parseServer(it, tokens));
		} else if (it->type == WORD) {
			std::string					val_key;
			std::vector<std::string>	val_values;
			
			val_key = it->value;
			it++;
			while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
				val_values.push_back(it->value);
				it++;
			}
			if (!val_key.empty())
			h.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
		} it++;
	} return h;
}

bool				ServerConfig::parse(std::ifstream& file) {
    tokens = tokenize(file);
    if(!isValidBraces(tokens) || !isValidSemicolon(tokens) || !isValidEncapsulation(tokens))
        return false;
	for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (it->type == KEYWORD && it->value == "http") {
			https.push_back(parseHttp(it, tokens));
		}
	}
	for (Http h : https) {
		for (ServerConfig_class scc : h.servers) {
			scc.mapToValues();
			scc.printValues();
			std::cout << std::endl;
		}
	}
    return true;
}
