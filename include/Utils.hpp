/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 22:10:28 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/28 22:10:28 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_UTILS_HPP
# define WEBSERV_UTILS_HPP

# include <vector>
# include <string>

/**
 * @brief Split string by delimiter(s)
 * 
 * @param string target to split
 * @param delims string defining delimiter(s) to use
 * @return std::vector<std::string> 
 */
std::vector<std::string> ft_splitstr(std::string string, std::string delims);

#endif  // WEBSERV_UTILS_HPP
