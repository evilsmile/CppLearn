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

if get_user_info_res.status == 200 then
    ngx.print(get_user_info_res.body);
end

if get_user_level_res.status == 200 then
    ngx.print(get_user_level_res.body);
end
