<?php
// Read input data from standard input (stdin)
$input = file_get_contents('php://stdin');

// Check if input data is not empty
if (!empty($input)) {
    // Get the content type from HTTP headers
    $contentType = isset($_SERVER['CONTENT_TYPE']) ? $_SERVER['CONTENT_TYPE'] : '';
    echo "Content-Type: $contentType\n"; // Debugging: Output content type

    // Check if the content type indicates a multipart/form-data request
    if (strpos($contentType, 'multipart/form-data') !== false) {
        // Extract boundary from content type
        preg_match('/boundary=(.*)$/', $contentType, $matches);
        $boundary = isset($matches[1]) ? $matches[1] : '';
        echo "Boundary: $boundary\n"; // Debugging: Output boundary

        // Split input data into parts using the boundary
        $parts = explode("--$boundary", $input);

        // Iterate over parts to find the image data
        foreach ($parts as $part) {
            // Remove leading/trailing whitespace and newlines
            $part = trim($part);
            echo "Part:\n$part\n"; // Debugging: Output part
            
            // Check if the part contains image data
            if (strpos($part, 'Content-Type: image/jpeg') !== false || strpos($part, 'Content-Type: image/png') !== false) {
                // Extract the image data
                preg_match('/\r\n\r\n(.*)\r\n--/', $part, $matches);
                $imageData = isset($matches[1]) ? $matches[1] : '';

                // Save the image data to a file
                $timestamp = time(); // Use current timestamp as part of the filename
                $filename = "./upload/image_$timestamp.jpg"; // Change extension as per image type
                file_put_contents($filename, base64_decode($imageData));
                echo "Image data found and saved to file: $filename\n"; // Debugging: Output success message

                // Output success message
                echo "Image uploaded successfully to $filename";
                exit; // Exit script after successful upload
            }
        }
    }
}

// If no image data was found or an error occurred, output an error message
echo "Error: Unable to process the image upload";
?>
