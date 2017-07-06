local user = ngx.var.user;

if not user then
    ngx.say("no user get when get info")
    ngx.exit(200)
end

local parser = require("redis.parser");
local res = ngx.location.capture("/redis", { 
        args = {
            cnt = 3,
            cmd = "hmget", 
            key = "User." .. user,
            field = "info";
        }
      });

if res.status == 200 then
   reply = parser.parse_reply(res.body);
   if not reply then
       ngx.say("user '", user, "' has no info!")
    else
       ngx.say(reply);
    end
end
