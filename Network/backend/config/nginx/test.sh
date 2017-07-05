#!/bin/bash


# Nginx - Lua 测试
#curl 'localhost:81/lua_calc?num1=23&num2=234'
#curl 'localhost:81/lua_hello?who=world'
#curl 'localhost:81/lua_file?a=23&b=834'

#curl 'localhost:81/lua_auth?user=ntes'
#curl 'localhost:81/lua_auth?user=soso'

#curl 'localhost:81/lua_call_subreq'

#curl 'localhost:81/lua_redis?key=mood'

#curl 'localhost:81/lua_ver'

#curl -A "Spalding sent HELLLO" 'localhost:81/lua_content_file/1/8' -d "post_param=333&post_param2=777"
curl -A "Spalding sent HELLLO" 'localhost:81/lua_content_file/1/8?arg_param=123&arg_param2=334' -d "post_param=333&post_param2=777"
