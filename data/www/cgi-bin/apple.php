<?php

$apples = isset($_POST['apples']) ? intval($_POST['apples']) : 0;
    $pricePerKilo = isset($_POST['price_per_kilo']) ? floatval($_POST['price_per_kilo']) : 0;

    // Calculate the total price
    $totalPrice = $apples * $pricePerKilo;

    // Output the total price
    echo "<h1>Order Summary</h1>";
    echo "<p>Number of Apples: $apples</p>";
    echo "<p>Price per Kilo: $pricePerKilo</p>";
    echo "<p>Total Price: $totalPrice</p>";

?>