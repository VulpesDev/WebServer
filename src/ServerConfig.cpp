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
    std::string   b, dump;
	std::string				dst;
	std::string::iterator	bpos, blast, start, src;

    found = 0;
    need_space = 0;
    last_space = 1;
    sharp_comment = 0;
    variable = 0;
    quoted = 0;
    s_quoted = 0;
    d_quoted = 0;

    b = cf->conf_file.buffer;
    dump = cf->conf_file.dump;
    start = b.begin();
    bpos = start;
    blast = b.end();
    start_line = cf->conf_file.line;

	std::ifstream fileStream((&cf->conf_file.file)->name.data(), std::ios::binary);
    file_size = fileLen(fileStream);

    std::cout << "I'm inside!" << std::endl;
    for ( ;; ) {

        if (bpos >= blast) {

            if (cf->conf_file.file.offset >= file_size) {

                if (cf->args.size() > 0 || !last_space) {

                    if (cf->conf_file.file.fd == INVALID_FILE) {
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
                cf->conf_file.line = start_line;

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
				std::copy(start, start + len, b.begin()); //questionable practice suggested by chat gpt
            }

            size = (ssize_t) (file_size - cf->conf_file.file.offset);

            if (size > blast - (b.begin() + len)) {
                size = blast - (b.begin() + len);
            }

            n = read_file(&cf->conf_file.file, b.begin() + len, size,
                              cf->conf_file.file.offset);

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

            bpos = b.begin() + len;
            blast = bpos + n -1;
            start = b.begin();

            // if (dump) {
            //     dump->last = ngx_cpymem(dump->last, b->pos, size);
            // }
        }

        ch = *(bpos++);

        if (ch == LF) {
            cf->conf_file.line++;

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
            std::cout<<"last space!" << std::endl;
            start = bpos - 1;
            start_line = cf->conf_file.line;

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
            std::cout << "found: " << found << std::endl;
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

char *
ngx_conf_parse(conf_t *cf, std::string filename)
{
    std::string             rv;
    int          fd;
    int         rc;
    buf_t         buf;
    conf_file_t  *prev, conf_file;
    enum {
        parse_file = 0,
        parse_block,
        parse_param
    } type;

    if (!filename.empty()) {

        /* open configuration file */
		std::ifstream file(DEFAULT_CONFIG_PATH, std::ios::binary);

        //prev = cf->conf_file;

		cf->conf_file.file.name = filename;
        cf->conf_file = conf_file;
        std::string line;
        while (std::getline(file, line))
        {
            cf->conf_file.buffer += line;
        }
        std::cerr << "PRINTING" << std::endl << cf->conf_file.buffer << std::endl << std::endl;

        buf.pos = buf.start;
        buf.last = buf.start;
        // buf.end = buf.last + NGX_CONF_BUFFER;
        // buf.temporary = 1;

        cf->conf_file.file.fd = fd;
        cf->conf_file.file.offset = 0;
        cf->conf_file.line = 1;

        type = parse_file;
	}


    for ( ;; ) {
        std::cout << "READING CONFIG!" << std::endl;
        rc = conf_read_token(cf);

        /*
         * ngx_conf_read_token() may return
         *
         *    NGX_ERROR             there is error
         *    NGX_OK                the token terminated by ";" was found
         *    NGX_CONF_BLOCK_START  the token terminated by "{" was found
         *    NGX_CONF_BLOCK_DONE   the "}" was found
         *    NGX_CONF_FILE_DONE    the configuration file is done
         */

        if (rc == ERROR) {
            goto done;
        }

        if (rc == CONF_BLOCK_DONE) {

            if (type != parse_block) {
                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "unexpected \"}\"");
				std::cerr << "unexpected \"}\"" << std::endl;
                goto failed;
            }

            goto done;
        }

        if (rc == CONF_FILE_DONE) {

            if (type == parse_block) {
                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                                //    "unexpected end of file, expecting \"}\"");
				std::cerr << "unexpected end of file, expecting \"}\"" << std::endl;
                goto failed;
            }
            std::cerr << "config file done!" << std::endl;
            goto done;
        }

        if (rc == CONF_BLOCK_START) {

            if (type == parse_param) {
                // ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                                //    "block directives are not supported "
                                //    "in -g option");
				std::cerr << "vlock directives are not supported in -g option" << std::endl;
                goto failed;
            }
        }

        /* rc == NGX_OK || rc == NGX_CONF_BLOCK_START */

        // if (cf->handler) {

        //     /*
        //      * the custom handler, i.e., that is used in the http's
        //      * "types { ... }" directive
        //      */

        //     if (rc == NGX_CONF_BLOCK_START) {
        //         ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "unexpected \"{\"");
        //         goto failed;
        //     }

        //     rv = (*cf->handler)(cf, NULL, cf->handler_conf);
        //     if (rv == NGX_CONF_OK) {
        //         continue;
        //     }

        //     if (rv == NGX_CONF_ERROR) {
        //         goto failed;
        //     }

        //     ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "%s", rv);

        //     goto failed;
        // }


        // rc = ngx_conf_handler(cf, rc);

        if (rc == ERROR) {
            goto failed;
        }
    }

failed:

    rc = ERROR;

done:

    if (!filename.empty()) {
        // if (cf->conf_file->buffer->start) {
        //     ngx_free(cf->conf_file->buffer->start);
        // }

        // if (ngx_close_file(fd) == NGX_FILE_ERROR) {
        //     ngx_log_error(NGX_LOG_ALERT, cf->log, ngx_errno,
        //                   ngx_close_file_n " %s failed",
        //                   filename->data);
        //     rc = NGX_ERROR;
        // }

        //cf->conf_file = *prev;
    }

    if (rc == ERROR) {
        //return NGX_CONF_ERROR;
		return "ERROR";
    }

    //return NGX_CONF_OK;
	return "CONFIG_OKAY";
}
