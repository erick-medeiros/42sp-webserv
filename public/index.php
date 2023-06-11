<?php

echo "Hello from PHP script!\n";

echo "SERVER_PROTOCOL: ";
echo getenv("SERVER_PROTOCOL");
echo "\n";

echo "SERVER_PORT:     ";
echo getenv("SERVER_PORT");
echo "\n";

echo "CONTENT_LENGTH:  ";
echo getenv("CONTENT_LENGTH");
echo "\n";

echo "HTTP_HOST:       ";
echo getenv("HTTP_HOST");
echo "\n";

echo "HTTP_ACCEPT:     ";
echo getenv("HTTP_ACCEPT");
echo "\n";

echo "REQUEST_METHOD:  ";
echo getenv("REQUEST_METHOD");
echo "\n";

echo "PATH_INFO:       ";
echo getenv("PATH_INFO");
echo "\n";

echo "SCRIPT_NAME:     ";
echo getenv("SCRIPT_NAME");
echo "\n";

echo "QUERY_STRING:    ";
echo getenv("QUERY_STRING");
echo "\n";
echo "\n";

$body = $argv[1];
echo $body;
echo "\n--------------------------------------\n";
echo "Request body payload size: ";
echo strlen($body);

?>
