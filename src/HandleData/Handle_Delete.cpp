#include "../../include/HandleData.hpp"

/// @brief handles the delete request
/// @param resource_path the uri passed in the request
/// @return the raw response message
std::string handle_delete_request(const Server server, const std::string& resource_path) {
    try {
        // Process the DELETE request based on the resource_path
        if (resource_path == "/delete") {
            std::string root = DEFAULT_PATH;
            for (auto it = server.locations.begin(); it != server.locations.end(); it++) {
                if (it->getPath() == resource_path) {
                    root = it->getRootedDir();
                }
            }
            std::string s = root + "uploaded_image.png";
            const char* file_to_delete = s.c_str();

            int result = std::remove(file_to_delete);
            if (result == 0) {
                // File deleted successfully
                HTTPResponse    h(200);
                return h.getRawResponse();
            } else {
                // Error deleting file
                return (check_error_page(server, resource_path, 503));
            }
        } else {
            return (check_error_page(server, resource_path, 404));
        }
    } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << std::endl; // Debug
        return (check_error_page(server, resource_path, 500));
    }
}