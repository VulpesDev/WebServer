<?php
// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get current working directory (cwd)
    $cwd = getcwd();
    echo "Current working directory: $cwd<br>";

    // Define upload directory path
    $uploadDir = $cwd . "/data/www/cgi-bin/upload";
    echo "Upload directory: $uploadDir<br>";

    // Get the filename to delete from POST parameters
    $postData = trim(file_get_contents('php://stdin'));
	echo "Raw POST data: $postData<br>";
 
    $filenameToDelete = substr($postData, strpos($postData, 'filename=') + 9);
    echo "Filename to delete: $filenameToDelete<br>";

    // Validate filename
    if ($filenameToDelete === '' or empty($filenameToDelete)) {
        echo "Error: Filename not provided.<br>";
        exit;
    }

    // Define the file path
    $filePath = $uploadDir . "/" . $filenameToDelete;

    // Check if the file exists
    if (!file_exists($filePath)) {
        echo "Error: File '$filenameToDelete' does not exist.<br>";
        exit;
    }

    // Attempt to delete the file
    if (unlink($filePath)) {
        echo "File '$filenameToDelete' deleted successfully.<br>";
    } else {
        echo "Error: Unable to delete file '$filenameToDelete'.<br>";
    }
} else {
    // Method not allowed
    http_response_code(405);
    echo "Error: Method not allowed<br>";
}
?>
