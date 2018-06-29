INCLUDE=-ICatch2/single_include/ -Iinclude
SRC=test/main.cpp test/ast.cpp test/json_parser.cpp

runtest: ${SRC} \
	include/surfsara/impl/object.hpp \
	include/surfsara/impl/array.hpp \
	include/surfsara/impl/node.hpp \
	include/surfsara/impl/json_parser.hpp \
	include/surfsara/impl/json_format.hpp \
	include/surfsara/ast.hpp \
	include/surfsara/json_parser.hpp \
	include/surfsara/json_format.hpp
	g++ -Wall -std=c++11 -fmax-errors=5  ${INCLUDE} -o runtest ${SRC}

