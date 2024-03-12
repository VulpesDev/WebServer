#include "../../include/HandleData.hpp"

/// @brief handles the delete request
/// @param resource_path the uri passed in the request
/// @return the raw response message
std::string handle_delete_request(const std::string& resource_path) {
    try {
        // Process the DELETE request based on the resource_path
        if (resource_path == "/delete") {
            const char* file_to_delete = "./data/www/uploaded_image.png";

            int result = std::remove(file_to_delete);
            if (result == 0) {
                // File deleted successfully
                HTTPResponse    h(200);
                return h.getRawResponse();
            } else {
                // Error deleting file
                int             err = 503;
                HTTPResponse    h(err);
	            h.setBody(generate_error_page(err));
                return h.getRawResponse();
            }
        } else {
            int             err = 404;
            HTTPResponse    h(err);
	        h.setBody(generate_error_page(err));
            return h.getRawResponse();
        }
    } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << std::endl; // Debug
        int             err = 500;
        HTTPResponse    h(err);
	    h.setBody(generate_error_page(err));
        return h.getRawResponse();
    }
}