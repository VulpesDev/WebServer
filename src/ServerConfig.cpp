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
	//print the saved values
	// for (Http h : https)
	// {
	// 	std::cout << "http: " << std::endl;
	// 	for(std::multimap<std::string, std::vector<std::string>>::iterator it = h.other_vals.begin(); it != h.other_vals.end(); it++) {
	// 		std::cout << it->first << ": ";
	// 		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
	// 			std::cout << *it2 << " ";
	// 		}
	// 		std::cout << std::endl;
	// 	}
	// 	for (Server s : h.servers){
	// 		std::cout << "server: " << std::endl;
	// 		for(std::multimap<std::string, std::vector<std::string>>::iterator it = s.other_vals.begin(); it != s.other_vals.end(); it++) {
	// 			std::cout << it->first << ": ";
	// 			for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
	// 				std::cout << *it2 << " ";
	// 			}
	// 			std::cout << std::endl;
	// 		}
	// 		for (Location l : s.locations){
	// 			std::cout << "location: " << l.path << std::endl;
	// 			for(std::multimap<std::string, std::vector<std::string>>::iterator it = l.other_vals.begin(); it != l.other_vals.end(); it++) {
	// 				std::cout << it->first << ": ";
	// 				for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
	// 					std::cout << *it2 << " ";
	// 				}
	// 				std::cout << std::endl;
	// 			}
	// 		}
	// 	}
	// }
    return true;
}

// int	ServerConfig::mapToStruct(Http& h)
// {
// 	//Not required by subject?
// }

// int	servName_validate_fill(otherVals_itc it, Server& s)
// {
// 	if (it->first == "server_name") {
// 		if (it->second.empty())			//check if the it->second is not empty
// 			return Err_ServerName;		//error then I'd also have to log it somehow
// 		s.server_name = it->second;
// 	}
// 	return 0;
// }

// int	maxBodySize_validate_fill(otherVals_itc it, Server& s)
// {
// 	if (it->first == "client_max_body_size") {
// 		std::string	val = it->second.at(0);							//possibly not existing
// 		int			numVal = std::atoi(it->second.at(0).c_str());	//maybe return error if this shit is 0
// 		char	c = val.back();

// 		if (val.empty())
// 			return Err_BodySize_Unit;
// 		if (numVal <= 0)
// 			return Err_BodySize_Numval;
// 		switch (c) {
// 		case K:
// 			numVal * K;
// 			break;
// 		case M:
// 			numVal * M;
// 			break;
// 		case G:
// 			numVal * G;
// 			break;
// 		default:
// 			return Err_BodySize_Unit;
// 		}
// 		s.max_body_size = numVal;
// 		return Warn_None, Err_None;
// 	}
// 	return Warn_BodySize_Missing;
// }

// int	host_port_validate_fill(otherVals_itc it, Server& s)
// {
// 	if (it->first == "listen") {
// 		int	result = std::atoi(it->second.at(0).c_str()); //return err if this is empty I guess
// 		if (result > 0 && result < MaxPortNum)
// 			s.port = result;
// 		else
// 			Err_Port_WrongParam;//error
// 		return Warn_None, Err_None;
// 	}
// 	return Warn_Port_Missing;
// }

// bool fileExists(const std::string& filePath) {
//     std::ifstream file(filePath);
//     return file.good();
// }

// bool isNumeric(const std::string& str) {
//     for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
//         if (!isdigit(*it)) {
//             return false;
//         }
//     }
//     return true;
// }

// int	errorPages_validate_fill(otherVals_itc it, Server& s)
// {
// 	int					error;
// 	ErrorPage			page;

// 	if (it->first == "error_page") {
// 		page.path = it->second.back();					// check if its null
// 		if (page.path.empty() || !fileExists(page.path))
// 			return Err_ErrPage_File;					//log error

// 		for (std::vector<std::string>::const_iterator i = it->second.begin();
// 		i != it->second.end(); i++) {
// 			if (*i != it->second.back())
// 				continue;
// 			if (!isNumeric(*i))
// 				continue;								//return an error
// 			error = std::atoi(i->c_str());
// 			if (error <= 0)
// 				continue;								//return an error
// 			page.errs.push_back(error);
// 		}
// 		s.err_pages.push_back(page);
// 		return Err_None, Warn_None;
// 	}
// 	return Warn_ErrPage_Missing;
// }

// int	ServerConfig::mapToStruct(Server& s)
// {
// 	//if the static bool is not set to true set it
// 	if (!s.dhp_set)
// 		s.dhp_set = true;
// 	for (otherVals_itc it = s.other_vals.begin(); it != s.other_vals.end(); it++) {
// 		//set server_name
// 		if (servName_validate_fill(it, s)) {
// 			//do error
// 		}
// 		//set max_bodysize
// 		if (maxBodySize_validate_fill(it, s)) {
// 			//do error
// 		}
// 		//set host and port
// 		if (host_port_validate_fill(it, s)) {
// 			//do error
// 		}
// 		//set error_pages
// 		if (errorPages_validate_fill(it, s)) {
// 			//do error
// 		}
// 	}
// }
