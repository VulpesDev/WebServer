<?php

function main(): int
{
	$sock = socket_create(AF_INET, SOCK_STREAM, 0);
	$conn = socket_connect($sock, "127.0.0.1", 8080);
	if (!$conn)
		return 1;
	socket_write($sock, "GET /index.html HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Test Client\r\nConnection: keep-alive\r\n", 84);
	sleep(1);
	socket_write($sock, "GET /compressed-data HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Test Client\r\nAccept-Encoding: gzip, deflate\r\nConnection: close\r\n", 114);
	socket_close($sock);
	return 0;
}

exit(main());