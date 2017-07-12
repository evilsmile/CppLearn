local user = ngx.var.arg_user;

if not user then
    ngx.say("no user get when handle");
    ngx.exit(200);
end

local get_user_info_res, get_user_level_res = ngx.location.capture_multi({
        {
            '/get_user_info/' .. user
        },
        {
            '/get_user_level/' .. user
        }
   }); 

if get_user_info_res.status ~= 200 or get_user_level_res.status ~= 200 then
    ngx.log(ngx.ERR, "user '", user, "' get info failed.");
    ngx.say("Oops, data corrupted");
    ngx.exit(200);
end

local userinfo = get_user_info_res.body;
local userlevel = get_user_level_res.body;

-- 根据第一位跳转
local userid_prefix = string.sub(userinfo, 1, 1);
local new_uri = "/old_user/" .. user;
if userid_prefix == "2" then
    new_uri = "/new_user/" .. user;
end

ngx.log(ngx.ERR, "new uri: ", new_uri);

-- 以下两种方式进行内部location 的跳转
-- 用于内部重定向，最好加上return，因为它不会主动返回
return ngx.exec(new_uri, "");

-- true 表示重新进行location匹配
-- ngx.req.set_uri(new_uri, true);
