#/bin/bash

# Nginx
cp -rf /usr/local/nginx/conf/nginx.conf nginx/
cp -rf /usr/local/nginx/conf/lua nginx/
cp -rf /usr/local/nginx/conf/servers nginx/

# Keepalived
cp -rf /opt/keepalived-1.3.5/etc/keepalived/keepalived.conf keepalived
cp -rf /opt/keepalived-1.3.5/etc/keepalived/chk_haproxy.sh keepalived
cp -rf /opt/keepalived-1.3.5/start.sh keepalived
cp -rf /etc/init.d/keepalived init.d/

# HAProxy
cp -rf /opt/haproxy-1.7.8/conf/haproxy.conf haproxy
cp -rf /opt/haproxy-1.7.8/errorfiles/ haproxy
cp -rf /opt/haproxy-1.7.8/start.sh haproxy
cp -rf /etc/init.d/haproxy init.d

# rsyslog
cp -rf /etc/rsyslog.conf .

cp -rf /etc/init.d/redis init.d/
