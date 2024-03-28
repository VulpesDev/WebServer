<?php
// Read input data from standard input (stdin)
$input = file_get_contents('php://stdin');
parse_str($input, $postData);
// $stdin = fopen('php://stdin', 'r');
// stream_set_blocking($stdin, false);

// $input = '';
// while (($char = fgetc($stdin)) !== false) {
//     $input .= $char;
//     usleep(100); // Sleep briefly to avoid busy-waiting
// }

// fclose($stdin);

// parse_str($input, $postData);
// Retrieve input data from the parsed POST data
$apples = isset($postData['apples']) ? intval($postData['apples']) : 0;
$pricePerKilo = isset($postData['price_per_kilo']) ? floatval($postData['price_per_kilo']) : 0;

// Calculate the total price
$totalPrice = $apples * $pricePerKilo;

// Output the result
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
