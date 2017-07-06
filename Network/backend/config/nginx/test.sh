#!/bin/bash

opt=""
if [ $# -gt 0 ]; then
    opt=$1
fi

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
#curl $opt -A "Spalding sent HELLLO" 'localhost:81/lua_content_file/1/8?arg_param=1 23&arg_param2=334' -d "post_param=333&post_param2=777"

#curl $opt 'localhost:81/lua_redirect'
#curl $opt 'localhost:81/lua_shared_data'

#curl 'localhost:81/lua_api_test'

#curl 'localhost:81/login'
curl 'localhost:81/login?user=evil'
