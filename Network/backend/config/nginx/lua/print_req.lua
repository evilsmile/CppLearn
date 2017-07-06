function print_t(t)
	for k, v in pairs(t) do
	    if type(v) == table then
	        ngx.say(k, ": ", table.concat(v), "<br/>");
	    else
	        ngx.say(k, ": ", v, "<br/>");
	    end
	end
end

---------- 写响应头 --------------
ngx.header.a = "Reply_A";
ngx.header.b = {"Reply_B1", "Reply_B2"};

-- 会以 [322] 结束, 不往下执行了。  
-- ngx.exit(322);

-- 设置响应码
ngx.status=303

---------- 写响应体 ------------
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


------------- other --------------
---- ngx.print 和 ngx.say 都是输出响应体，但是ngx.print不打印最后的换行符
ngx.print("http version: ", ngx.req.http_version(), "<br/>");
ngx.print("http get_method ", ngx.req.get_method(), "<br/>");
ngx.print("http raw_header ", ngx.req.raw_header(), "<br/>");
ngx.print("http body data ", ngx.req.get_body_data(), "<br/>");

local request_uri = ngx.var.request_uri;
ngx.say("ngx request uri: ", request_uri, "<br/>");
ngx.say("unescape uri: ", ngx.unescape_uri(request_uri), "<br/>");
ngx.say("md5(123): ", ngx.md5("123"), "<br/>");
ngx.say("http time: ", ngx.http_time(ngx.time()), "<br/>");
ngx.say("encode args ", ngx.encode_args({a1="arg1", a2="arg2"}), "<br/>");

local decoded_uri=ngx.decode_args("arg1=day1&arg2= monday");
print_t(decoded_uri);

local base64_encode = ngx.encode_base64("321");
ngx.say("base64 encode: ", base64_encode);
ngx.say("base64 decode: ", ngx.decode_base64(base64_encode));


-- ngx.redirect("http://www.baidu.com", 302);
