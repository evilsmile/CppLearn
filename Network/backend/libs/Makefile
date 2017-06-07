CXXFLAGS= -g -I../include -I./

all:liblog.a libutil.a 

.a.o:
	ar r -o $@ $<

liblog.a:log.o 
	ar r -o $@ $^

libutil.a:stringutil.o	api_mysql.o
	ar r -o $@ $^

clean:
	$(RM) -rf *.o lib*.a
