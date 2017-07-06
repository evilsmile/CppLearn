local cmd = ngx.var.cmd;
local key = ngx.var.key;
local field = ngx.var.field;

if not cmd or cmd == "" then
    ngx.say("Oops, server error!");
    ngx.log(ngx.ERR, "redis cmd missed");
    ngx.exit(500)
end

local parser = require("redis.parser");

local args = {cnt = 1, cmd = cmd};
if key and key ~= "" then
    args.cnt = args.cnt + 1;
    args["key"] = key;

	if field and field ~= "" then
	    ngx.log(ngx.ERR, "field: ", field);
	    args.cnt = args.cnt + 1;
	    args["field"] = field;
	end
end

local capture_param = { args = args };

local res = ngx.location.capture("/redis", capture_param);

if res.status == 200 then
   reply = parser.parse_reply(res.body);
   if not reply then
       ngx.say("No result from in redis");
    else
       ngx.say(reply);
    end
end
