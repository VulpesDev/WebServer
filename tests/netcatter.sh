#!/usr/bin/env bash

REQUEST="GET / HTTP/1.1\r\nUser-Agent: netcat\r\nConnection: Close\r\n\r\n"

echo -e ${REQUEST} | nc 127.0.0.1 8080