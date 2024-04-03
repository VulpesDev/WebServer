<?php
// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get current working directory (cwd)
    $cwd = getcwd();
    echo "Current working directory: $cwd\n";

    // Define upload directory path
    $uploadDir = $cwd . "/data/www/cgi-bin/upload";

    // Check if the upload directory exists, create it if not
    if (!file_exists($uploadDir)) {
        mkdir($uploadDir, 0777, true); // Recursive directory creation
    }

    // Check if the request contains multipart form data
    if (isset($_SERVER['CONTENT_TYPE']) && preg_match('/boundary=(.*)$/', $_SERVER['CONTENT_TYPE'], $matches)) {
        // Extract the boundary string
        $boundary = $matches[1];

        // Read the raw input from stdin
        $raw_data = file_get_contents('php://stdin');

        // Parse the multipart form data
        $parts = explode('--' . $boundary, $raw_data);

        // Remove empty parts and the last boundary
        array_pop($parts);
        array_shift($parts);

        // Loop through each part
        foreach ($parts as $part) {
            // Separate headers and content
            list($raw_headers, $body) = explode("\r\n\r\n", trim($part), 2);

            // Parse headers
            $raw_headers .= "\r\n"; // Ensure ending newline character
            preg_match_all('/(.*?): (.*?)\r\n/', $raw_headers, $matches, PREG_SET_ORDER);
            $headers = [];
            foreach ($matches as $match) {
                $headers[$match[1]] = $match[2];
            }

            // Check if the part contains a file
            if (isset($headers['Content-Disposition']) && strpos($headers['Content-Disposition'], 'filename=') !== false) {
                // Extract filename from Content-Disposition header
                preg_match('/filename="(.+?)"/', $headers['Content-Disposition'], $filename_matches);
                $filename = $filename_matches[1];

                // Extract file content
                $file_content = $body;

                // Define destination path in upload directory
                $destination = $uploadDir . "/" . $filename;

                // Save the file
                if (file_put_contents($destination, $file_content) === false) {
                    echo "Error: Unable to save file $filename\n";
                } else {
                    // Provide a link to access the uploaded file
                    $fileUrl = "http://localhost:8080/cgi-bin/upload/$filename";
                    echo "File uploaded successfully: <a href='$fileUrl'>$filename</a>\n";
                }
            }
        }
    } else {
        // Invalid Content-Type header
        http_response_code(400);
        echo "Error: Invalid Content-Type header\n";
    }
} else {
    // Method not allowed
    http_response_code(405);
    echo "Error: Method not allowed\n";
}
echo "<p><a href=\"/cgi-bin/\">Back to CGI Home</a></p>\n";

?>
