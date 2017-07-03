#!/bin/bash


# Nginx - Lua 测试
curl 'localhost/lua_calc?num1=23&num2=234'
curl 'localhost/lua_hello?who=world'
curl 'localhost/lua_file?a=23&b=834'

curl 'localhost/lua_auth?user=ntes'
#curl 'localhost/lua_auth?user=soso'

curl 'localhost/lua_call_subreq'
