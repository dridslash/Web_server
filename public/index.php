<?php

// Set the variables for the HTML content
$title = 'My PHP-Generated HTML Page';
$message = 'Hello, World!';

// Generate the HTML content
$html = "<!DOCTYPE html>
<html>
<head>
	<title>$title</title>
</head>
<body>
	<h1>$message</h1>
</body>
</html>";

// Output the HTML content
echo $html;
?>