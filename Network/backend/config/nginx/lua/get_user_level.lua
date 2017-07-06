local user = ngx.var.user;

if not user then
    ngx.say("no user get when get info")
    ngx.exit(200)
end

local parser = require("redis.parser");
local res = ngx.location.capture("/redis", { 
        args = {
                cnt = 3,
                cmd="get", 
                key = "User.Level" .. user,
                field = "level"
            }
      });

if res.status == 200 then
   reply = parser.parse_reply(res.body);
   if not reply then
       ngx.say("user '", user, "' has no level!")
    else
       ngx.say(reply);
    end
end
