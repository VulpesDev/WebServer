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

# include <algorithm>
# include <fstream>
# include <iostream>
# include <map>
# include <string>
# include <sstream>
# include <vector>

# include <Utils.hpp>

// case-insensitive string comparison functor,
// ref. S. Meyers, "Effective STL"
struct CIStringLess : \
	public std::binary_function<std::string, std::string, bool>
{
	struct CIStringCompare : \
		public std::binary_function<unsigned char, unsigned char, bool>
	{
		bool operator()(unsigned char const &lhs, unsigned char const &rhs) \
			const;
	};

	bool operator()(std::string const &lhs, std::string const &rhs) const;
};

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

		typedef std::map<std::string, std::string, CIStringLess> HeaderMap;

		std::string	raw_;
		std::string	start_line_;
		std::string	version_;
		std::string	header_;
		std::string	payload_;
		HeaderMap	headers_;
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
		std::string const &method() const;
		std::string	const &URI() const;
		std::string const &get_header(std::string const &key) const;

		// Constants
		static std::string const	methods[];

	private:
		bool validate_method();

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
