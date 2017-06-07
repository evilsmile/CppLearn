CXXFLAGS=-g -I./include

all:server test.cgi 

server:server.cpp libs/liblog.a libs/libutil.a
	$(CXX) $(CXXFLAGS) -o $@ $^ -lmysqlclient

test.cgi:test_cgi.cpp libs/liblog.a 
	$(CXX) $(CXXFLAGS) -o $@ $^ -lcgicc -lfcgi

install:
	cp -f test.cgi /usr/local/nginx/web_dev/cgi-bin/

clean:
	$(RM) -f server 
