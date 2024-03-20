#include "HandleData.hpp"
#include <dirent.h>

/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message



static std::string handle_auto_index(const Server server, const std::string& resource_path) {
    std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
                         "<title>webserv</title></head><body><h1>webserv</h1><h2>Index of Index";
    // std::cerr << "this is resource path ----" << resource_path << std::endl;
    // result += resource_path;
    (void)resource_path;
    result += "<hr><div>";
    // Location loc;

    DIR *dir = NULL;
    // std::string full_path = "/home/rtimsina/Desktop/cursu_working/11_webserv/webserv_17.03/data/www";// + resource_path;
    std::string full_path = server.locations[0].getRootedDir().c_str();
    std::cerr << "full path: " << full_path << std::endl;
    dir = opendir(full_path.c_str());
    std::cerr << "handling auto index" << std::endl;
    if (!dir) {
        std::cerr << " Error \n handling auto index" << std::endl;
        return "";
    }
    struct dirent *file = NULL;
    while ((file = readdir(dir)) != NULL) {
        if (file->d_name[0] != '.') {
            result += "<a href=\"" + resource_path + file->d_name + "\">" + file->d_name + "</a><br>";
            if (file->d_type == DT_DIR) {
                result += "/";
            }
            result += "<br>";
        }
    }

    closedir(dir);

    result += "</div></body></html>";

    HTTPResponse resp(200);

    resp.setHeader("Content-Type", "text/html");
    resp.setHeader("Connection", "close");
    resp.setBody(result);
    resp.setHeader("Content-Length", std::to_string(result.size()));
    return(resp.getRawResponse());
    std::cerr << "handling auto index finished" << std::endl;
    // Assuming you have a method to send the response
    // server.sendResponse(resp);
}









/* static std::string handle_auto_index(const Server server, const std::string& resource_path) {
    std::cerr << "Auto index" << std::endl; //debug
	std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
		"<title>webserv</title></head><body><h1>webserv</h1><h2>Index of Index";
	result += resource_path;
    // (void)resource_path;
	result += "<hr><div>";

    DIR *dir = NULL;
    if ((resource_path == "/") || (resource_path == "")) {
        dir = opendir("./data/www");
    } else {
        dir = opendir(( "./data/www" + resource_path).c_str());
    }
    if (!dir) {
        std::cerr << " Error \n handling auto index" << std::endl;
        return "";
    }
    struct dirent *file = NULL;
    while ((file = readdir(dir)) != NULL) {
        if (file->d_name[0] != '.') {
            result += "<a href=\"/" + resource_path + file->d_name + "\">" + file->d_name + "</a><br>";
        }
        else if (resource_path[resource_path.size() - 1] != '/') {
            result += "<a href=\"/" + resource_path + file->d_name + "\">" + file->d_name + "</a><br>";
        }
        else {
            result += "<a href=\"" + resource_path + file->d_name + "\">" + file->d_name + "</a><br>";
        }
        result += (file->d_type == DT_DIR ? "/" : "") +  (std::string)"\">";
        result += (std::string)(file->d_name) + (file->d_type == DT_DIR ? "/" : "") + "</a><br>";
    }
    closedir(dir);

    result += "</div></body></html>";

    HTTPResponse resp(200);

    resp.setHeader("Content-Type", "text/html");
    resp.setHeader("Connection", "close");
    resp.setBody(result);
    resp.setHeader("Content-Length", std::to_string(result.size()));
    return(resp.getRawResponse());

    // return "";
} */

std::string handle_get_request(const Server server, const std::string& resource_path) {
    std::cerr << "Handling get request" << std::endl; //debug
    std::ifstream file(( "./data/www" + resource_path).c_str());

    Location location;
    std::cerr << "Getting location" << std::endl; //debug
    std::cerr << location.getAutoIndex() << std::endl;
    if (location.getAutoIndex()) { //check if auto index is on do directory listing
        return(handle_auto_index(server, resource_path));
        // return "";
    }

    else if (file.is_open()) {
        std::cerr << "Opening file" << std::endl; //debug
        // Read the contents of the file
        std::stringstream file_contents;
        file_contents << file.rdbuf();
        std::cerr << "Creating a response" << std::endl; //debug

        // Build the HTTP response
        HTTPResponse    h(200);
        h.setBody(file_contents.str());
        file.close();

        return (h.getRawResponse());
    } else {
        // Resource not found
        std::cerr << "Not found" << std::endl; //debug
        return (check_error_page(server, resource_path, 404));
    }

    return "";
}
