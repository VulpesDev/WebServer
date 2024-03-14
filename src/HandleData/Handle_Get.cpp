#include "HandleData.hpp"

/// @brief Handles the get request
/// @param resource_path is the uri passed in the request
/// @return the raw response message
std::string handle_get_request(const std::string& resource_path) {

    std::ifstream file(( "./data/www" + resource_path).c_str());

    if (file.is_open()) {
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
        int             err = 404;
        HTTPResponse    h(err);

	    h.setBody(generate_error_page(err));

        return (h.getRawResponse());
    }

    return "";
}