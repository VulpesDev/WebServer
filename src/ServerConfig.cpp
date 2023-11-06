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
