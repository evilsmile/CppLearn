local count = 0;
local delay = 3;

local heartbeatCheck = nil;

heartbeatCheck = function(args) 
   count = count + 1;
   if count > 3 then
       return
   end

   ngx.log(ngx.ERR, "Timer check task.~~~~~~~~~");
   local ok, err = ngx.timer.at(delay, heartbeatCheck);

   if not ok then
     ngx.log(ngx.ERR, "start timer task failed", err);
   end
end

heartbeatCheck()
