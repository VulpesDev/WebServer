/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 16:51:41 by mcutura           #+#    #+#             */
/*   Updated: 2023/10/28 16:51:41 by mcutura          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVERCONFIG_HPP
# define WEBSERV_SERVERCONFIG_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <vector>

# define CONF_BUFFER  4096

# define  INVALID_FILE         -1
# define  FILE_ERROR           -1

# define  OK          0
# define  ERROR      -1
# define  AGAIN      -2
# define  BUSY       -3
# define  DONE       -4
# define  DECLINED   -5
# define  ABORT      -6


#define CONF_BLOCK_START 1
#define CONF_BLOCK_DONE  2
#define CONF_FILE_DONE   3

#define LF     (u_char) '\n'
#define CR     (u_char) '\r'
#define CRLF   "\r\n"

typedef struct buf_s {
    std::string::iterator          pos;
    std::string::iterator          last;
    off_t            file_pos;
    off_t            file_last;

    std::string::iterator          start;         /* start of buffer */
    std::string::iterator          end;           /* end of buffer */
    file_t      *file;
    buf_t       *shadow;
} buf_t;

typedef struct file_s {
    int                   fd;
    std::string            name;
    off_t                      offset;
    off_t                      sys_offset;

    //ngx_log_t                 *log;
} file_t;

typedef struct {
    file_t            file;
    buf_t            *buffer;
    buf_t            *dump;
    uint32_t            line;
} conf_file_t;


typedef struct conf_s {
    std::string	name;
    std::vector<void *>	args;
	conf_file_t      *conf_file;
} conf_t;

static std::string const DEFAULT_CONFIG_PATH = "./data/webserv.default.conf";

class ServerConfig
{
	public:
		ServerConfig();
		ServerConfig(std::string const &config_path);
		ServerConfig(ServerConfig const &other);
		ServerConfig const &operator=(ServerConfig const &rhs);
		~ServerConfig();

		bool is_valid() const;

	private:
		bool parse(std::ifstream &file);

		bool	is_valid_;
};

#endif  // WEBSERV_SERVERCONFIG_HPP
