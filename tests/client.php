<?php

function main(): int
{
	$sock = socket_create(AF_INET, SOCK_STREAM, 0);
	$conn = socket_connect($sock, "127.0.0.1", 14242);
	if (!$conn)
		return 1;
	socket_write($sock, "GET /index.html HTTP/1.1\r\n", 26);
	sleep(1);
	socket_write($sock, "POST /submit-data HTTP/1.1\r\nHost: intra.42.fr\r\nUser-Agent: My HTTP Client\r\nContent-Type: application/json\r\nContent-Length: 42\r\nConnection: keep-alive\r\n\r\n{\"name\": \"John\", \"age\": 30}\r\n", 182);
	sleep(1);
	socket_write($sock, "GET /compressed-data HTTP/1.1\r\nHost: intra.42.fr\r\nUser-Agent: My HTTP Client\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n", 129);
	sleep(1);
	socket_close($sock);
	return 0;
}

exit(main());