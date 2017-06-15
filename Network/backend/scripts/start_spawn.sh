#!/bin/bash

CGI_DIR=/usr/local/nginx/web_dev/cgi-bin/
SPAWN_BIN=/usr/local/nginx/sbin/spawn-fcgi 

function restart_cgi()
{
    PORTS=$1
    CGI_NAME=$2

    pid=$(netstat -lpn | egrep -w "($PORTS)" | awk '{print $NF}' | sed 's!/.*!!g');
    kill -9 $pid;

    echo "$PORTS" | sed 's/|/\n/' | while read port  ; do
        $SPAWN_BIN -a 127.0.0.1 -p $port -f $CGI_DIR/${CGI_NAME}
    done
}

function restart_gate()
{
    CGI_NAME=gate.cgi 
    PORTS="8088|8089"
    
    restart_cgi $PORTS $CGI_NAME
}

restart_gate
