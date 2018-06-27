/*
MIT License

Copyright (c) 2018 SURFsara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <catch2/catch.hpp>
#include <surfsara/ast.hpp>

using namespace surfsara::ast;


TEST_CASE("test integer atoms", "[Object]")
{
  Node n1(1);
  Node n2(2);
  Node n3(1);
  REQUIRE(n1.isA<Integer>());
  REQUIRE(n1.isA<Object>() == false);
  REQUIRE(n1.isA<Array>() == false);
  REQUIRE(n1 != n2);
  REQUIRE(n1 == n1);
  REQUIRE(n1 == n3);
  REQUIRE(n1.as<Integer>() == 1);
}

TEST_CASE("different constructors", "[Node]")
{
  Node n1(1);
  Node n2(1.0);
  Node n3(true);
  Node n4(Null());
  Node n5("abc");
  Node n6({Pair("abc",1), Pair("def", 2)});
  Node n7({"abc",1, "def", 2});
  Node n8, n9, n10;
  n8 = {Pair("abc", 1), Pair("def", 2), Pair("xyz", {2, 3})};
  n9 = {"abc",1, "def", 2};
  n10 = "adf";
  auto obj = Object{{"a", 1}};
}

TEST_CASE("test object", "[Object]")
{
  Object obj{{"name", "John"}, {"age", 30}, {"car", Null()}};
  obj.set("lastname", "Smith");
  REQUIRE(obj.has("name"));
  REQUIRE(obj.has("lastname"));
  REQUIRE(obj.has("__") == false);
  REQUIRE(obj.modify("age", [](Node & node){ node = 31; }));
  REQUIRE(obj.modify("__", [](Node & node){ node = 31; }) == false);
  obj.forEach([](const std::string & key, Node & node){ if(key == "age") node = 32; });
  REQUIRE(obj.remove("__") == false);
  REQUIRE(obj.remove("lastname"));
  REQUIRE(obj.remove([](const std::string & key, const Node & node) { return node == Null(); }));
}
