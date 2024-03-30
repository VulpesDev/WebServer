#include "HandleData.hpp"

/**
 * Check if the method access is allowed for a given path on the server.
 *
 * @param server the server object
 * @param path the path to check access for
 * @param method the method to check access for
 *
 * @return true if access is allowed, false otherwise
 *
 */
bool    check_method_access(Server server, std::string path, std::string method) {

    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == path) {
            std::vector<std::string> accepted_methods = it->getAcceptedMethods();

            if (accepted_methods.size() == 0) {
                return true;
            }
            for (std::vector<std::string>::const_iterator accepted_method = accepted_methods.begin();
                 accepted_method != accepted_methods.end();
                 ++accepted_method) {
                std::cerr << *accepted_method << " ";
            }

            for (std::vector<std::string>::const_iterator accepted_method = accepted_methods.begin();
                 accepted_method != accepted_methods.end();
                 ++accepted_method) {
                if ((*accepted_method) == method) {
                    return true;
                }
            }
            return false;
        }
    }
    return true;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        return "";
    }
}

/**
 * Checks the error page for a given server, path, and error code.
 *
 * @param server the server to check
 * @param path the path to the error page
 * @param error_code the error code to check
 *
 * @return the HTTP response with the error page, or a generated error page
 * 
 */
std::string check_error_page(Server server, int error_code) {
    HTTPResponse resp(error_code);
    errPages_arr err_pages = server.GetErrPages();
    for (errPages_arr::const_iterator it = err_pages.begin(); it != err_pages.end(); ++it) {
        for (std::vector<int>::const_iterator err_code = it->errs.begin(); err_code != it->errs.end(); ++err_code) {
            if ((*err_code) == error_code) {
                std::string error_page = readFile(it->path);
                if (error_page != "") {
                    resp.setBody(error_page);
                    return resp.getRawResponse();
                }
            }
        }
    }
    resp.setBody(generate_error_page(error_code));
    return resp.getRawResponse();
}

/**
 * Checks if the given path requires a redirection in the server's configuration.
 *
 * @param server The server object containing the locations configuration.
 * @param path The path to check for redirection.
 *
 * @return The raw response string containing the redirection information if a redirection is found, otherwise an empty string.
 * 
 */
std::string check_redirection(Server server, std::string path) {
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == path && !it->getRedirection().text.empty()) {
            HTTPResponse resp(it->getRedirection().status);
            resp.setHeader("Location", it->getRedirection().text);
            return resp.getRawResponse();
        }
    }
    return "";
}

/**
 * Checks the directory root for a matching location path and updates the request path if found.
 *
 * @param server The server object containing the list of locations.
 * @param req The HttpRequest object representing the current request.
 *
 * @return void
 * 
 */
void    check_directory_root(Server server, HttpRequest& req) {
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == req.getPath() && !it->getRootedDir().empty()) {
            req.setPath(it->getRootedDir() + req.getPath());
            std::cerr << "NEW PATH: " << req.getPath() << std::endl;
            return;
        }
    }
}

/**
 * Check if the given path is a directory.
 *
 * @param path The path to check.
 *
 * @return True if the path is a directory, false otherwise.
 * 
 */
bool is_directory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

/**
 * Check if the requested path matches a directory index file in the server locations.
 *
 * @param server the server object containing locations to check
 * @param req the HTTP request object to modify
 *
 * @return void
 * 
 */
void    check_directory_index(Server server, HttpRequest& req) {
    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (is_directory(it->getPath()) && it->getPath() == req.getPath() && !it->getIndexFile().empty()) {
            req.setPath(req.getPath() + it->getIndexFile());
            std::cerr << "NEW PATH: " << req.getPath() << std::endl;
            return;
        }
    }
}

/**
 * Check the size of the request body against the maximum body size allowed by the server.
 *
 * @param server The server object
 * @param req The HttpRequest object
 *
 * @return An empty string if the body size is within the limit, otherwise an error page
 *
 * @throws None
 */
std::string    check_body_size(Server server, HttpRequest& req) {
    if (req.getBody().size()/1024 <= server.GetMaxBodySize()) {
        return "";
    }
    return check_error_page(server, 413);
}

/**
 * Handles various checks for a given server and HTTP request.
 *
 * @param server the server to handle the request checks for
 * @param req the HTTP request to handle the checks for
 *
 * @return a string representing the result of the checks
 *
 * @throws ErrorType if an error occurs during the checks
 */
std::string handle_request_checks(Server& server, HttpRequest& req) {
    if (req.getHttpVersion() != "HTTP/1.1"){
        return (check_error_page(server, 505));
    }
    check_directory_index(server, req);
    std::string check_redir = check_redirection(server, req.getPath());
    if (!check_redir.empty()) {
        return check_redir;
    }
    std::string check_body = check_body_size(server, req);
    if (!check_body.empty()) {
        return check_body;
    }
    return "";
}
