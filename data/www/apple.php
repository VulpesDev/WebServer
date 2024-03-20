<?php
// Retrieve input data from environment variables
$apples = isset($_ENV['apples']) ? intval($_ENV['apples']) : 0;
$pricePerKilo = isset($_ENV['price_per_kilo']) ? floatval($_ENV['price_per_kilo']) : 0;

// Calculate the total price
$totalPrice = $apples * $pricePerKilo;

// Output the result
echo "Content-Type: text/html\n\n"; // Set the Content-Type header
echo "<!DOCTYPE html>\n";
echo "<html lang=\"en\">\n";
echo "<head>\n";
echo "<meta charset=\"UTF-8\">\n";
echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
echo "<title>Apple Order Result</title>\n";
echo "</head>\n";
echo "<body>\n";
echo "<h1>Apple Order Result</h1>\n";
echo "<p>Number of Apples: $apples</p>\n";
echo "<p>Price per Kilo: $pricePerKilo</p>\n";
echo "<p>Total Price: $totalPrice</p>\n";
echo "</body>\n";
echo "</html>\n";
?>
