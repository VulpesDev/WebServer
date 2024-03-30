#include "HandleData.hpp"

/// @brief gets the boundary of the multipart form data
/// @param content_type the line of the header part of the request
/// @return the boundary as a std::string
std::string get_boundary(const std::string& content_type) {
    std::string boundary_tag = "boundary=";
    size_t pos = content_type.find(boundary_tag);
    if (pos != std::string::npos) {
        return content_type.substr(pos + boundary_tag.length());
    }
    throw std::runtime_error("Boundary not found in Content-Type header");
}

/// @brief extracts the data from png content by searching the start/end boundary
/// @param file_content raw body content
/// @param boundary the boundary returned by get_boundary()
/// @return raw data pf the png
std::string extract_png_data(const std::string& file_content, const std::string& boundary) {
    std::string png_data;
    std::string boundary_str = "--" + boundary;
    std::string boundary_end_str = "--" + boundary + "--";

    size_t start_pos = file_content.find(boundary_str);
    if (start_pos == std::string::npos) {
        throw std::runtime_error("Start boundary not found");
    }
    start_pos = file_content.find("\r\n\r\n", start_pos);
    if (start_pos == std::string::npos) {
        throw std::runtime_error("End of headers not found");
    }
    start_pos += 4;

    size_t end_pos = file_content.find(boundary_end_str, start_pos);
    if (end_pos == std::string::npos) {
        throw std::runtime_error("End boundary not found");
    }

    png_data = file_content.substr(start_pos, end_pos - start_pos);

    return png_data;
}

/// @brief handles the post request
/// @param resource_path the uri in the request
/// @param file_content the body of the request 
/// @return the raw response message
std::string handle_post_request(const Server server, const std::string& resource_path, const std::string& file_content) {
    try {
        // Process the POST request based on the resource_path
        if (resource_path == "/upload") {
            // Parse the Content-Type header to get the boundary
            std::string content_type = "Content-Type: multipart/form-data; boundary=";
            std::string boundary = get_boundary(content_type);

            // Extract PNG data from the multipart form-data
            std::string png_data = extract_png_data(file_content, boundary);

            // Save the PNG data to a file
            std::string root = DEFAULT_PATH;
            for (std::vector<Location>::const_iterator it = server.locations.begin(); it != server.locations.end(); it++) {
                if (it->getPath() == resource_path) {
                    root = it->getRootedDir();
                }
            }
            std::ofstream file((root + "/uploaded_image.png").c_str(), std::ios::binary);
            file.write(png_data.c_str(), png_data.size());
            file.close();

            // Return a success response
            HTTPResponse    h(200);
            return h.getRawResponse();
        } else {
            // Handle other POST requests to unknown endpoints
            return (check_error_page(server, 404));
        }
    } catch (const std::exception& e) {
        // Handle any errors
        // std::cerr << "Internal error: " << e.what() << std::endl; // Debug
        return (check_error_page(server, 500));
    }
}
