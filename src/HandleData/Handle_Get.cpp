#include "HandleData.hpp"
#include <dirent.h>

/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message


// static std::string handle_auto_index(const Server server, const std::string& resource_path) {
//     std::cerr << "Auto index" << std::endl; //debug
// 	std::string result = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
// 		"<title>webserv</title></head><body><h1>webserv</h1><h2>Index of Index";
// 	result += resource_path;
//     std::cerr << "resource path: " << resource_path << std::endl;
// 	result += "<hr><div>";

//     DIR *dir = NULL;
//     if ((resource_path == "/") || (resource_path == "")) {
//         dir = opendir("./data/www");
//     } else {
//         dir = opendir(("./data/www" + resource_path).c_str());
//     }
//     if (!dir) {
//         std::cerr << " Error \n handling auto index" << std::endl;
//         return "";
//     }

//     if (resource_path != "/") {
//         std::string parent_path = resource_path.substr(0, resource_path.find_last_of("/"));
//         if (parent_path.empty()) parent_path = "/";
//         result += "<a href=\"" + parent_path + "\" style=\"color:red\">[Go Back]</a><br>";
//     }

//     struct dirent *file = NULL;
//     while ((file = readdir(dir)) != NULL) {
//         if (file->d_name[0] != '.') {
//             std::string file_path = resource_path + file->d_name;
//             if (resource_path[resource_path.size() - 1] != '/') {
//                 file_path = resource_path + "/" + file->d_name;
//             }
//             result += "<a href=\"" + file_path + "\">" + file->d_name + "</a>";
//             if (file->d_type == DT_DIR) {
//                 result += "/";
//             }
//             result += "<br>";
//         }
//     }
//     closedir(dir);


//     result += "</div></body></html>";

//     HTTPResponse resp(200);

//     resp.setHeader("Content-Type", "text/html");
//     resp.setHeader("Connection", "close");
//     resp.setBody(result);
//     resp.setHeader("Content-Length", std::to_string(result.size()));
//     return(resp.getRawResponse());
// } 

static std::string handle_auto_index(const Server server, const std::string& resource_path) {
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
        dir = opendir("./data/www");
    } else {
        dir = opendir(("./data/www" + resource_path).c_str());
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
    resp.setHeader("Content-Length", std::to_string(result.size()));
    return resp.getRawResponse();
}

std::string handle_get_request(const Server server, const std::string& resource_path) {
    // std::cerr << "Handling get request" << std::endl; //debug
    std::string root = DEFAULT_PATH;
    for (auto it = server.locations.begin(); it != server.locations.end(); it++) {
        if (it->getPath() == resource_path) {
            root = it->getRootedDir();
        }
    }
    std::ifstream file(( root + resource_path).c_str());

    Location location;
    std::cerr << "Getting location autoindex:- " << server.locations[3].getAutoIndex() << std::endl; //debug
    if (server.locations[3].getAutoIndex() && resource_path.find_first_of('.') == std::string::npos) { //check if auto index is on do directory listing
        return(handle_auto_index(server, resource_path));
    }

    else if (file.is_open()) {
        std::cerr << "Opening file" << std::endl; //debug
        // Read the contents of the file
        std::stringstream file_contents;
        file_contents << file.rdbuf();
        // std::cerr << "Creating a response" << std::endl; //debug

        // Build the HTTP response
        HTTPResponse    h(200);
        h.setBody(file_contents.str());
        file.close();

        return (h.getRawResponse());
    } else {
        // Resource not found
        // std::cerr << "Not found: " << root + resource_path << std::endl; //debug
        return (check_error_page(server, resource_path, 404));
    }

    return "";
}
