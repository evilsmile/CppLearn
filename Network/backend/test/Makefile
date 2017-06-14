LIB64_DIR=/usr/local/lib64/
JSON_LIB=${LIB64_DIR}/libjsoncpp.a
LOG_LIB=../libs/libutil.a

DEP_LIBS=${JSON_LIB} ${LOG_LIB}
CXX=g++
CXXFLAGS=-g -I../include -I../libs
LDFLAGS=-lcurl

all:test

test:test.cpp ${DEP_LIBS}
