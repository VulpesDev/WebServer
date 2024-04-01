#include "HandleData.hpp"
#include <dirent.h>

/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message

static std::string handle_auto_index(const Server server, const std::string& resource_path, std::string root) {
    (void)server;
    std::cerr << "Auto index" << std::endl; //debug
    std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
        "<title>webserv</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #f4f4f4; font-size: 16px; }"
        ".container { max-width: 800px; margin: 0 auto; padding: 20px; background-color: #fff; border: 1px solid #ccc; border-radius: 5px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); }"
        "h1 { color: #333; }"
        "h2 { color: #666; }"
        "a { text-decoration: none; }"
        "a.file { color: #007bff; }"
        "a.directory { color: #28a745; }"
        "a.back { color: #dc3545; }"
        ".directory-list { list-style-type: none; padding: 0; }"
        ".directory-list li { margin-bottom: 5px; font-size: 18px; }"
        "</style>"
        "</head><body><div class=\"container\"><h1>webserv</h1><h2>Index of " + resource_path + "</h2><hr><ul class=\"directory-list\">";

    DIR *dir = NULL;
    if ((resource_path == "/") || (resource_path == "")) {
        dir = opendir(root.c_str());
    } else {
        std::cerr << "root + path: " << root + resource_path << std::endl;
        dir = opendir((root + resource_path).c_str());
    }
    if (!dir) {
        std::cerr << " Error \n handling auto index" << std::endl;
        return "";
    }

    if (resource_path != "/") {
        std::string parent_path = resource_path.substr(0, resource_path.find_last_of("/"));
        if (parent_path.empty()) parent_path = "/";
        result += "<li><a href=\"" + parent_path + "\" class=\"back\">[Go Back]</a></li>";
    }

    struct dirent *file = NULL;
    while ((file = readdir(dir)) != NULL) {
        if (file->d_name[0] != '.') {
            std::string file_path = resource_path + file->d_name;
            if (resource_path[resource_path.size() - 1] != '/') {
                file_path = resource_path + "/" + file->d_name;
            }
            result += "<li><a href=\"" + file_path + "\" class=\"" + ((file->d_type == DT_DIR) ? "directory" : "file") + "\">" + file->d_name;
            if (file->d_type == DT_DIR) {
                result += "/";
            }
            result += "</a></li>";
        }
    }
    closedir(dir);

    result += "</ul></div></body></html>";

    HTTPResponse resp(200);
    resp.setHeader("Content-Type", "text/html");
    resp.setHeader("Connection", "close");
    resp.setBody(result);
    char buf[20];
    sprintf(buf, "%zu", result.size());
    resp.setHeader("Content-Length", buf);
    return resp.getRawResponse();
}

std::string handle_get_request(const Server server, const std::string& resource_path) {
    std::cerr << "Handling get request" << std::endl; //debug
    std::cerr << "Resource path: " << resource_path << std::endl; //debug
    std::string root = DEFAULT_PATH;

    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++) {
        if (it->getPath() == resource_path) {
            root = it->getRootedDir();
        }
    }

    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        std::cerr << "getpath: " << it->getPath() << " | resource_path: " << resource_path << std::endl; //debug
        if (it->getPath() == resource_path && it->getAutoIndex()) {
            std::cerr << "Getting location autoindex:- " << it->getPath() << " - " << it->getAutoIndex() << std::endl; //debug
            std::cerr << "Auto index" << std::endl; //debug
            return(handle_auto_index(server, resource_path, root));
        }
    }

    std::ifstream file(( root + resource_path).c_str());
    std::cerr << "Trying to open file: " << root + resource_path << std::endl; //debug
    if (file.is_open()) {
        std::cerr << "Opening file" << std::endl; //debug
        // Read the contents of the file
        std::stringstream file_contents;
        file_contents << file.rdbuf();

        // Build the HTTP response
        HTTPResponse    h(200);
        h.setBody(file_contents.str());
        file.close();

        return (h.getRawResponse());
    } else {
        // Resource not found
        std::cerr << "--Not found------" << std::endl; //debug
        return (check_error_page(server, 404));
    }

    return "";
}
