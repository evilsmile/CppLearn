#!/usr/bin/lua


local luasql = require "luasql.mysql"
local redis = require "redis"

env = assert(luasql.mysql())

mysql_conn = env:connect("es", "root", "123Naruto", "127.0.0.1", 3306)

local redis_cli = redis.connect({
    host = "127.0.0.1",
    port = 6379
})

cur = mysql_conn:execute("SELECT name, userno, level FROM es.users")

row = cur:fetch({}, "a")
while row do
    userinfo = row.userno .. "|" .. row.name;
    level = row.level
    key = "User." .. row.name
    print ("Adding " .. key)
    local hashKV = {
        info = userinfo,
        level = level
    }
    redis_cli:hmset(key, hashKV)
    row = cur:fetch(row, "a")
end
