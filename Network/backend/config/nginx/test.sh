#!/bin/bash

opt=""
if [ $# -gt 0 ]; then
    opt=$@
fi

curl_cmd="curl $opt"

# Nginx - Lua 测试
function do_test()
{
#    $curl_cmd 'localhost:82/lua_shared_data'
#    $curl_cmd 'localhost:82/main'
#    $curl_cmd 'localhost:82/lua_rewrite'
    
   # $curl_cmd 'localhost:82/lua_api_test'
    #$curl_cmd 'localhost:82/bar'
    $curl_cmd 'localhost:82/global_var_test?name=ian&date=20170707'  -d "Big chunk post data" -A "New-generation-useragent" -H "Cookie: session=77788"

}


# -------- 正式用法 ------
function do_the_thing()
{
    $curl_cmd 'localhost:81/login?user=old_evil'
    $curl_cmd 'localhost:81/login?user=evil'
}

do_test

#do_the_thing
