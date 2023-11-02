/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 23:03:11 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/27 23:03:11 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HTTPMESSAGE_HPP
# define WEBSERV_HTTPMESSAGE_HPP

# include <fstream>
# include <iostream>
# include <string>
# include <sstream>
# include <vector>

# include <Utils.hpp>

class AHttpMessage
{
	public:
		virtual ~AHttpMessage();

		// Getters
		std::string const &version() const;
		std::string const &raw() const;

	protected:
		AHttpMessage();
		AHttpMessage(std::string const &raw);
		AHttpMessage(AHttpMessage const &other);
		AHttpMessage const &operator=(AHttpMessage const &rhs);

		bool		is_complete_;
		std::string	raw_;
		std::string	start_line_;
		std::string	version_;
		std::string	header_;
		std::string	payload_;
};

class Request : public AHttpMessage
{
	public:
		Request();
		Request(std::string const &raw);
		Request(Request const &other);
		Request const &operator=(Request const &rhs);
		~Request();

		void append(std::string const &tail);
		int validate_start_line();

		// Getters
		bool is_complete() const;
		std::string const &	method() const;
		std::string	const &	URI() const;

	private:
		std::string	method_;
		std::string	URI_;
};

class Reply : public AHttpMessage
{
	public:
		Reply(std::string const &version, int status);
		~Reply();

		static std::string const get_status_message(int status);

	private:
		bool	is_sent_;
		int		status_code_;


		/* = delete */
		Reply();
		Reply(Reply const &other);
		Reply const &operator=(Reply const &rhs);
};

#endif  // WEBSERV_HTTPMESSAGE_HPP
