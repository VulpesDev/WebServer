/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 19:17:43 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/28 19:17:43 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpMessage.hpp>

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---

AHttpMessage::AHttpMessage()
{
	this->version_ = "HTTP/1.1";  // default supported version
}

AHttpMessage::AHttpMessage(std::string const &raw) : raw_(raw)
{
	this->version_ = "HTTP/1.1";  // default supported version
}

AHttpMessage::AHttpMessage(AHttpMessage const &other)
{
	*this = other;
}

AHttpMessage const &AHttpMessage::operator=(AHttpMessage const &rhs)
{
	if(this == &rhs)
		return *this;
	this->raw_ = rhs.raw_;
	this->start_line_ = rhs.start_line_;
	this->version_ = rhs.version_;
	this->header_ =rhs.header_;
	this->payload_ = rhs.payload_;
	return *this;
}

AHttpMessage::~AHttpMessage() 
{}

////////////////////////////////////////////////////////////////////////////////
// --- GETTERS ---

std::string const &AHttpMessage::version() const
{
	return this->version_;
}

std::string const &AHttpMessage::raw() const
{
	return this->raw_;
}

////////////////////////////////////////////////////////////////////////////////
// --- EXTRAS ---

bool CIStringLess::CIStringCompare::operator() \
	(unsigned char const &lhs, unsigned char const &rhs) const
{
	return std::tolower(lhs) < std::tolower(rhs);
}

bool CIStringLess::operator() \
	(std::string const &lhs, std::string const &rhs) const
{
	return std::lexicographical_compare(lhs.begin(), lhs.end(), \
		rhs.begin(), rhs.end(), CIStringCompare());
}
