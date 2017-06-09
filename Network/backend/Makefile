CXXFLAGS=-g -I./include

all:server-1 test.cgi 

server-1:server1.cpp libs/liblog.a libs/libutil.a include/config.h
	$(CXX) $(CXXFLAGS) -o $@ $^ -lmysqlclient

test.cgi:test_cgi.cpp libs/liblog.a 
	$(CXX) $(CXXFLAGS) -o $@ $^ -lcgicc -lfcgi

install:
	cp -f test.cgi /usr/local/nginx/web_dev/cgi-bin/

clean:
	$(RM) -f server 
