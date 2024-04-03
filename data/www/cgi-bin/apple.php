<?php
$input = file_get_contents('php://stdin');
parse_str($input, $postData);
$apples = isset($postData['apples']) ? intval($postData['apples']) : 0;
$pricePerKilo = isset($postData['price_per_kilo']) ? floatval($postData['price_per_kilo']) : 0;

$totalPrice = $apples * $pricePerKilo;

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
echo "<p><a href=\"/cgi-bin/\">Back to CGI Home</a></p>\n";
echo "</body>\n";
echo "</html>\n";
?>
