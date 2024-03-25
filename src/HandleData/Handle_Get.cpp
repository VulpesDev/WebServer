#include "HandleData.hpp"

/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message
std::string handle_get_request(const Server server, const std::string& resource_path) {
    // std::cerr << "Handling get request" << std::endl; //debug
    std::string root = DEFAULT_PATH;
    for (auto it = server.locations.begin(); it != server.locations.end(); it++) {
        if (it->getPath() == resource_path) {
            root = it->getRootedDir();
        }
    }
    std::ifstream file(( root + resource_path).c_str());

    if (file.is_open()) {
        // std::cerr << "Opening file" << std::endl; //debug
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