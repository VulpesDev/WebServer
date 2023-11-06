/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/25 02:04:45 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/25 02:04:45 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>

#include <Server.hpp>

int	main(void)
{
	Server webserv;

	if (!webserv.initialize())
		return 1;
	std::signal(SIGINT, Server::stop);
	webserv.start();
	return 0;
}
