CXXFLAGS= -g -I../include -I./

all:liblog.a libutil.a 

.a.o:
	ar r -o $@ $<

liblog.a:log.o 
	ar r -o $@ $^

libutil.a:stringutil.o	api_mysql.o xmlparser.o
	ar r -o $@ $^

test:test.cpp  libutil.a liblog.a 
	$(CXX) ${CXXFLAGS} -o $@ $^ -ltinyxml2
	./test
	
clean:
	$(RM) -rf *.o lib*.a
