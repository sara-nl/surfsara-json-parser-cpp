INCLUDE=-ICatch2/single_include/ -Iinclude
SRC=test/main.cpp test/ast.cpp

runtest: test/main.cpp test/ast.cpp include/surfsara/ast.hpp
	g++ -Wall -std=c++11 ${INCLUDE} -o runtest ${SRC}
