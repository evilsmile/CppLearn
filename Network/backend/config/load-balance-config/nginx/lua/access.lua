local user = ngx.var.arg_user;

if not user then
    ngx.say("Need param 'user'");
elseif user == "evil" or user == "new_evil" then
    return;
else
    ngx.say("user '", user, "' not allowed");
    ngx.exit(200);
end

