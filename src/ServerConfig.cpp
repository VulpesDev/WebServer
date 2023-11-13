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

Server				parseServer(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Server s;
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
			if (!val_key.empty())
			{
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
	//print the saved values
	for (Http h : https)
	{
		std::cout << "http: " << std::endl;
		for(std::multimap<std::string, std::vector<std::string>>::iterator it = h.other_vals.begin(); it != h.other_vals.end(); it++) {
			std::cout << it->first << ": ";
			for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
				std::cout << *it2 << " ";
			}
			std::cout << std::endl;
		}
		for (Server s : h.servers){
			std::cout << "server: " << std::endl;
			for(std::multimap<std::string, std::vector<std::string>>::iterator it = s.other_vals.begin(); it != s.other_vals.end(); it++) {
				std::cout << it->first << ": ";
				for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
					std::cout << *it2 << " ";
				}
				std::cout << std::endl;
			}
			for (Location l : s.locations){
				std::cout << "location: " << l.path << std::endl;
				for(std::multimap<std::string, std::vector<std::string>>::iterator it = l.other_vals.begin(); it != l.other_vals.end(); it++) {
					std::cout << it->first << ": ";
					for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
						std::cout << *it2 << " ";
					}
					std::cout << std::endl;
				}
			}
		}
	}
    return true;
}

int	ServerConfig::mapToStruct(Http& h)
{
	//Not required by subject?
}

int	servName_validate_fill(otherVals_itc it, Server& s)
{
	if (it->first == "server_name") {
		s.server_name = it->second;									//Maybe check if the it->second is not empty
	}
	return 0;
}

int	maxBodySize_validate_fill(otherVals_itc it, Server& s)
{
	if (it->first == "client_max_body_size") {
		std::string	val = it->second.at(0);							//possibly not existing
		int			numVal = std::atoi(it->second.at(0).c_str());	//maybe return error if this shit is 0
		char	c = val.back();
		switch (c) {
		case K:
			numVal * K;
			break;
		case M:
			numVal * M;
			break;
		case G:
			numVal * G;
			break;
		default:
			break;
		}
		if (numVal == 0) //should return an error
			;
		s.max_body_size = numVal;
	}
	return 0;
}

int	host_port_validate_fill(otherVals_itc it, Server& s)
{
	int	result = std::atoi(it->second.at(0).c_str()); //return err if this is empty I guess
	if (result > 0 && result < MaxPortNum)
		s.port = result;
	else
		;//error
	return 0;
}

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

bool isNumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return false;
        }
    }
    return true;
}

int	errorPages_validate_fill(errPages_itc it, Server& s)
{
	std::string	path;
	std::vector<int> errors;

	path = it->second.back(); // check if its null
	if (path.empty() || !fileExists(path))
		;//error
	
	//fill in the errors array
	// for (size_t i = 0; i < count; i++) {
	// 	/* code */
	// }
	
}

int	ServerConfig::mapToStruct(Server& s)
{
	//if the static bool is not set to true set it
	if (!s.dhp_set)
		s.dhp_set = true;
	for (otherVals_itc it = s.other_vals.begin(); it != s.other_vals.end(); it++) {
		//set server_name
		if (servName_validate_fill(it, s)) {
			//do error
		}
		//set max_bodysize
		if (maxBodySize_validate_fill(it, s)) {
			//do error
		}
		//set host and port
		if (host_port_validate_fill(it, s)) {
			//do error
		}
		//set error_pages
		if (errorPages_validate_fill(it, s)) {
			//do error
		}
	}
}
