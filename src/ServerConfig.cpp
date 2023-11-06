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

std::streampos	fileLen(std::ifstream &file)
{
	file.seekg(0, std::ios::end);
	std::streampos length = file.tellg();
	file.seekg(0, std::ios::beg);
	return (length);
}

ssize_t read_file(file_t *file, std::string::iterator buf, size_t size, off_t offset)
{
    std::ifstream fileStream(file->name.data(), std::ios::binary);

    if (!fileStream.is_open()) {
        // ngx_log_error(NGX_LOG_ERR, file->log, ngx_errno,
        //               "Failed to open file: \"%s\"", file->name.data);
        return ERROR;
    }

    // Use the seekg function to set the file offset
    fileStream.seekg(offset, std::ios::beg);

    fileStream.read(reinterpret_cast<char*>(*buf), size);
    ssize_t n = fileStream.gcount();

    if (fileStream.bad() || fileStream.fail()) {
        // ngx_log_error(NGX_LOG_ERR, file->log, ngx_errno,
        //               "Failed to read from file: \"%s\"", file->name.data);
        return ERROR;
    } else if (n == 0) {
        // EOF reached
        return 0;
    }
    file->offset = offset + n;
    return n;
}

static intptr_t
conf_read_token(conf_t *cf)
{
    u_char      ch;
    std::streampos        file_size;
    size_t       len;
    ssize_t      n, size;
    uint32_t   found, need_space, last_space, sharp_comment, variable, start_line;
    uint32_t   quoted, s_quoted, d_quoted;
    std::string   word;
    buf_t   *b, *dump;
	std::string				dst;
	std::string::iterator	bpos, start, src;

    found = 0;
    need_space = 0;
    last_space = 1;
    sharp_comment = 0;
    variable = 0;
    quoted = 0;
    s_quoted = 0;
    d_quoted = 0;

    b = cf->conf_file->buffer;
    dump = cf->conf_file->dump;
    start = b->pos;
    start_line = cf->conf_file->line;

	std::ifstream fileStream((&cf->conf_file->file)->name.data(), std::ios::binary);
    file_size = fileLen(fileStream);

    for ( ;; ) {

        if (bpos >= b->last) {

            if (cf->conf_file->file.offset >= file_size) {

                if (cf->args.size() > 0 || !last_space) {

                    if (cf->conf_file->file.fd == INVALID_FILE) {
                        // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                        //                    "unexpected end of parameter, "
                        //                    "expecting \";\"");
						std::cerr << "unexpected end of parameter, expecting \";\"" << std::endl;
                        return ERROR;
                    }

                    // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                                    //    "unexpected end of file, "
                                    //    "expecting \";\" or \"}\"");
						std::cerr << "unexpected end of file, expecting \";\" or \"}\"" << std::endl;
                    return ERROR;
                }

                return CONF_FILE_DONE;
            }

            len = bpos - start;

            if (len == CONF_BUFFER) {
                cf->conf_file->line = start_line;

                if (d_quoted) {
                    ch = '"';

                } else if (s_quoted) {
                    ch = '\'';

                } else {
                    // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                    //                    "too long parameter \"%*s...\" started",
                    //                    10, start);
					std::cerr << "too long parameter \"" << *start << "\" started" << std::endl;
                    return ERROR;
                }

                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                //                    "too long parameter, probably "
                //                    "missing terminating \"%c\" character", ch);
				std::cerr << "too long parameter, probably missing terminating \"" << ch << "\" character" << std::endl;
                return ERROR;
            }

            if (len) {
				std::copy(start, start + len, b->start); //questionable practice suggested by chat gpt
            }

            size = (ssize_t) (file_size - cf->conf_file->file.offset);

            if (size > b->last - (b->start + len)) {
                size = b->last - (b->start + len);
            }

            n = read_file(&cf->conf_file->file, b->start + len, size,
                              cf->conf_file->file.offset);

            if (n == ERROR) {
                return ERROR;
            }

            if (n != size) {
                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                //                    ngx_read_file_n " returned "
                //                    "only %z bytes instead of %z",
                //                    n, size);
				std::cerr << "returned only " << n << "bytes instead of" << size << std::endl;
                return ERROR;
            }

            bpos = b->start + len;
            b->last = bpos + n;
            start = b->start;

            // if (dump) {
            //     dump->last = ngx_cpymem(dump->last, b->pos, size);
            // }
        }

        ch = (bpos++)[0];

        if (ch == LF) {
            cf->conf_file->line++;

            if (sharp_comment) {
                sharp_comment = 0;
            }
        }

        if (sharp_comment) {
            continue;
        }

        if (quoted) {
            quoted = 0;
            continue;
        }

        if (need_space) {
            if (ch == ' ' || ch == '\t' || ch == CR || ch == LF) {
                last_space = 1;
                need_space = 0;
                continue;
            }

            if (ch == ';') {
                return OK;
            }

            if (ch == '{') {
                return CONF_BLOCK_START;
            }

            if (ch == ')') {
                last_space = 1;
                need_space = 0;

            } else {
				//!all of those comments, to be replaced by logging
                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                //                    "unexpected \"%c\"", ch);
				std::cerr << "unexpected \"" << ch << "\"" << std::endl;
                return ERROR;
            }
        }

        if (last_space) {

            start = bpos - 1;
            start_line = cf->conf_file->line;

            if (ch == ' ' || ch == '\t' || ch == CR || ch == LF) {
                continue;
            }

            switch (ch) {

            case ';':
            case '{':
                if (cf->args.size() == 0) {
                    // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                    //                    "unexpected \"%c\"", ch);
					std::cerr << "unexpected \"" << ch << "\"" << std::endl;
                    return ERROR;
                }

                if (ch == '{') {
                    return CONF_BLOCK_START;
                }

                return OK;

            case '}':
                if (cf->args.size() != 0) {
                    // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                    //                    "unexpected \"}\"");
					std::cerr << "unexpected \"" << "}" << "\"" << std::endl;
                    return ERROR;
                }

                return CONF_BLOCK_DONE;

            case '#':
                sharp_comment = 1;
                continue;

            case '\\':
                quoted = 1;
                last_space = 0;
                continue;

            case '"':
                start++;
                d_quoted = 1;
                last_space = 0;
                continue;

            case '\'':
                start++;
                s_quoted = 1;
                last_space = 0;
                continue;

            case '$':
                variable = 1;
                last_space = 0;
                continue;

            default:
                last_space = 0;
            }

        } else {
            if (ch == '{' && variable) {
                continue;
            }

            variable = 0;

            if (ch == '\\') {
                quoted = 1;
                continue;
            }

            if (ch == '$') {
                variable = 1;
                continue;
            }

            if (d_quoted) {
                if (ch == '"') {
                    d_quoted = 0;
                    need_space = 1;
                    found = 1;
                }

            } else if (s_quoted) {
                if (ch == '\'') {
                    s_quoted = 0;
                    need_space = 1;
                    found = 1;
                }

            } else if (ch == ' ' || ch == '\t' || ch == CR || ch == LF
                       || ch == ';' || ch == '{')
            {
                last_space = 1;
                found = 1;
            }

            if (found) {
                for (src = start, len = 0;
                     src < bpos - 1;
                     len++)
                {
                    if (*src == '\\') {
                        switch (src[1]) {
                        case '"':
                        case '\'':
                        case '\\':
                            src++;
                            break;

                        case 't':
                            dst += '\t';
                            src += 2;
                            continue;

                        case 'r':
                            dst += '\r';
                            src += 2;
                            continue;

                        case 'n':
                            dst += '\n';
                            src += 2;
                            continue;
                        }

                    }
                    dst += *(src++);
                }
                // *dst = '\0'; //I dont think it should be null terminated
				std::cout<< "RESULT: " << dst << std::endl;

                if (ch == ';') {
                    return OK;
                }

                if (ch == '{') {
                    return CONF_BLOCK_START;
                }

                found = 0;
            }
        }
    }
}
