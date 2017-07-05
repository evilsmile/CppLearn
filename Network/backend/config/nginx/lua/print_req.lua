function print_t(t)
	for k, v in pairs(t) do
	    if type(v) == table then
	        ngx.say(k, ": ", table.concat(v), "<br/>");
	    else
	        ngx.say(k, ": ", v, "<br/>");
	    end
	end
end

local var = ngx.var;

ngx.say("ngx var a: ", var.a, "<br/>");
ngx.say("ngx var b: ", var.b, "<br/>");
ngx.say("ngx var[2]: ", var[2], "<br/>");
ngx.var.b = 2;

ngx.say("<br/>");

-------------- headers --------
local headers = ngx.req.get_headers();
ngx.say("headers begin: <br/>");
ngx.say("Host: ", headers["Host"], "<br/>");
ngx.say("User-agent: ", headers["user-agent"], "<br/>");
ngx.say("User-agent: ", headers.user_agent, "<br/>");

print_t(headers);
ngx.say("headers end<br>");

-------------- get params --------
ngx.say("uri args begin", "<br/>");
local uri_args = ngx.req.get_uri_args();
for k, v in pairs(uri_args) do
    if type(v) == table then
        ngx.say(k, ": ", table.concat(v), "<br/>");
    else
        ngx.say(k, ": ", v, "<br/>");
    end
end
ngx.say("uri args end", "<br/>");

-------------- post params --------
ngx.say("post args begin", "<br/>");
-- NOTE! --
ngx.req.read_body();
local post_args = ngx.req.get_post_args();
for k, v in pairs(post_args) do
    if type(v) == table then
        ngx.say(k, ": ", table.concat(v), "<br/>");
    else
        ngx.say(k, ": ", v, "<br/>");
    end
end
ngx.say("post args end", "<br/>");


ngx.say("http version: ", ngx.req.http_version(), "<br/>");
ngx.say("http get_method ", ngx.req.get_method(), "<br/>");
ngx.say("http raw_header ", ngx.req.raw_header(), "<br/>");
ngx.say("http body data ", ngx.req.get_body_data(), "<br/>");
