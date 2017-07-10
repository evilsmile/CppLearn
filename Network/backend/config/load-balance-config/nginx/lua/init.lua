----- check Jit version ---
if jit then
    ngx.log(ngx.ALERT, "Using Lua JIT: [", jit.version, "]")
else
    ngx.log(ngx.ALERT, "Using Lua interpreter: [", _VERSION, "]")
end


