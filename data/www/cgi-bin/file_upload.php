<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['file'])) {
    $file = $_FILES['file'];

    // Check for errors during file upload
    if ($file['error'] !== UPLOAD_ERR_OK) {
        echo "Error uploading file. Error code: " . $file['error'];
        exit();
    }

    // Determine the destination directory (server's temporary directory)
    $uploadDir = sys_get_temp_dir() . '/uploads/';

    // Create the uploads directory if it doesn't exist
    if (!file_exists($uploadDir)) {
        mkdir($uploadDir, 0777, true); // Recursive directory creation
    }

    // Move the uploaded file to the uploads directory
    $destination = $uploadDir . basename($file['name']);
    if (move_uploaded_file($file['tmp_name'], $destination)) {
        echo "File uploaded successfully to $destination";
    } else {
        echo "Error uploading file. Please try again.";
    }
}
?>
