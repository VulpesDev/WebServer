#include "HandleData.hpp"

std::string handle_get_request(const Server server, const std::string& resource_path) {
    std::string root = DEFAULT_PATH;

    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++) {
        if (it->getPath() == resource_path) {
            root = it->getRootedDir();
        }
    }

    for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); ++it) {
        if (it->getPath() == resource_path && it->getAutoIndex()) {
            return(handle_auto_index(server, resource_path, root));
        }
    }

    std::ifstream file(( root + resource_path).c_str());
    if (file.is_open()) {
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
        return (check_error_page(server, 404));
    }

    return "";
}
