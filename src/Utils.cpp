/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 21:25:32 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/28 21:25:32 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Utils.hpp>

std::vector<std::string> ft_splitstr(std::string string, std::string delims)
{
	std::vector<std::string>	split;
	std::string::size_type		start(0);
	std::string::size_type		end(0);

	while ((end = string.find_first_of(delims, start)) != std::string::npos) {
		if (end > start) {
			split.push_back(string.substr(start, end - start));
			start = end;
		}
		if (++start == string.size())
			break;
	}
	if (end == std::string::npos && start < string.size())
		split.push_back(string.substr(start, string.size()));
	return split;
}

#ifdef UNIT_TESTS
# include <iostream>

// TODO rewrite as proper unit tests that don't require manual inspection
int main(int, char **)
{
	std::string					str;
	std::vector<std::string>	vec;

	str = " What is love? Baby don't hurt me! Baby don't hurt me, no more. OK";
	vec = ft_splitstr(str, " ,.!?");
	if (vec.size() != 14)
		return 1;
	typedef std::vector<std::string>::iterator iter;
	for (iter it = vec.begin(); it != vec.end(); ++it)
		std::cout << *it << std::endl;
	return 0;
}

#endif
