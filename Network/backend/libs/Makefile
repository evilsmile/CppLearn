CXXFLAGS= -g -I../include -I./

CPPS=$(shell ls *.cpp)
OBJS=$(patsubst %.cpp,%.o,${CPPS})

all:libutil.a 

.a.o:
	ar r -o $@ $<

libutil.a:${OBJS}
	ar r -o $@ $^

test:test.cpp  libutil.a 
	$(CXX) ${CXXFLAGS} -o $@ $^ -ltinyxml2
	./test
	
clean:
	$(RM) -rf *.o lib*.a
