#!/bin/bash

function chk_haproxy_proc_cnt()
{
    local cnt=$(ps -C haproxy --no-header | wc -l)
    echo $cnt
}

if (( $(chk_haproxy_proc_cnt) == 0 )) ; then
    service haproxy start
    sleep 3
    if (($(chk_haproxy_proc_cnt) == 0)); then
        echo "Start haproxy failed. Stop keepalived";
        killall keepalived;
    fi
fi
