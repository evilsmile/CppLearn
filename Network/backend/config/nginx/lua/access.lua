local user = ngx.var.arg_user;

if not user then
    ngx.say("Need param 'user'");
elseif user == "evil" then
    return;
else
    ngx.say("user '", user, "' not allowed");
end

ngx.exit(200);
