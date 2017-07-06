#!/bin/bash

opt=""
if [ $# -gt 0 ]; then
    opt=$@
fi

# Nginx - Lua 测试
function do_test()
{
#    curl $opt 'localhost:82/lua_shared_data'
#    curl $opt 'localhost:82/main'
    curl $opt 'localhost:82/lua_rewrite'

   # curl 'localhost:82/lua_api_test'
}


# -------- 正式用法 ------
function do_the_thing()
{
    curl 'localhost:81/login?user=evil'
}

do_test

#do_the_thing
