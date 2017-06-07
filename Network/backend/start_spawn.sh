#!/bin/bash

pid=$(netstat -lpn | grep -w 8088 | awk '{print $NF}' | sed 's!/.*!!g');

kill -9 $pid;

/usr/local/nginx/sbin/spawn-fcgi -a 127.0.0.1 -p 8088 -f /usr/local/nginx/web_dev/cgi-bin/test.cgi
