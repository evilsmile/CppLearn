#!/bin/bash


# Nginx - Lua 测试
#curl 'localhost:81/lua_calc?num1=23&num2=234'
#curl 'localhost:81/lua_hello?who=world'
#curl 'localhost:81/lua_file?a=23&b=834'

#curl 'localhost:81/lua_auth?user=ntes'
#curl 'localhost:81/lua_auth?user=soso'

#curl 'localhost:81/lua_call_subreq'

curl 'localhost:81/lua_redis?key=mood'
