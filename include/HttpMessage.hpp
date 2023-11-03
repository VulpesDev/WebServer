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
# include <map>
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

		std::string	raw_;
		std::string	start_line_;
		std::string	version_;
		std::string	header_;
		std::map<std::string, std::string>	headers_;
		std::string	payload_;
};

class HttpRequest : public AHttpMessage
{
	public:
		HttpRequest();
		HttpRequest(std::string const &raw);
		HttpRequest(HttpRequest const &other);
		HttpRequest const &operator=(HttpRequest const &rhs);
		~HttpRequest();

		void append(std::string const &tail);
		int validate_start_line();
		bool parse_headers();

		// Getters
		bool is_complete() const;
		std::string const &	method() const;
		std::string	const &	URI() const;

	private:
		std::string	method_;
		std::string	URI_;
};

class HttpReply : public AHttpMessage
{
	public:
		HttpReply(int status);
		~HttpReply();

		static std::string const get_status_message(int status);

	private:
		bool	is_sent_;
		int		status_code_;

		void get_payload(std::string const &path, std::string &payload);
		std::string const generate_error_page(int status);

		/* = delete */
		HttpReply();
		HttpReply(HttpReply const &other);
		HttpReply const &operator=(HttpReply const &rhs);
};

#endif  // WEBSERV_HTTPMESSAGE_HPP
