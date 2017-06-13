LIB64_DIR=/usr/local/lib64/
JSON_LIB=${LIB64_DIR}/libjsoncpp.a
LOG_LIB=../libs/liblog.a

DEP_LIBS=${JSON_LIB} ${LOG_LIB}
CXX=g++
CXXFLAGS=-g -I../include -I../libs

all:test

test:test.cpp ${DEP_LIBS}
