all: runtest
	echo "done"

INCLUDE=-ICatch2/single_include/ -Iinclude
SRC=	test/main.cpp\
	test/ast.cpp\
	test/json_parser.cpp\
	test/json_parser_impl.cpp

DEP= 	include/surfsara/impl/object.hpp \
	include/surfsara/impl/array.hpp \
	include/surfsara/impl/node.hpp \
	include/surfsara/impl/json_format.hpp \
	include/surfsara/ast.h \
	include/surfsara/json_parser.h \
	include/surfsara/json_format.h

runtest: ${SRC} ${DEP} include/surfsara/impl/json_parser.hpp
	g++ -g -Wall -std=c++11 -fmax-errors=5  ${INCLUDE} -o runtest ${SRC}
