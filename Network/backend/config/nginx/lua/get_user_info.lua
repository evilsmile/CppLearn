local parser = require("redis.parser");
local res = ngx.location.capture("/redis", { args = { key = ngx.var.arg_user }});

if res.status == 200 then
   reply = parser.parse_reply(res.body);
   if not reply then
       ngx.say("user not found!")
    else
     ngx.say(reply);
    end
end
