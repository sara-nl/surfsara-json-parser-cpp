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
#include <surfsara/ast.h>
#include <surfsara/json_format.h>

using namespace surfsara::ast;

TEST_CASE("test null atom", "[Node]")
{
  Null null;
  Node n;
  REQUIRE(n.isA<Null>()    == true);
  REQUIRE(n.isA<Integer>() == false);
  REQUIRE(n.isA<Boolean>() == false);
  REQUIRE(n.isA<Float>()   == false);
  REQUIRE(n.isA<String>()  == false);
  REQUIRE(n.isA<Array>()   == false);
  REQUIRE(n.isA<Object>()  == false);

  REQUIRE(n == n);
  REQUIRE(n.as<Null>() == null);

  REQUIRE(formatJson(n) == "null");
}

TEST_CASE("test integer atom", "[Node]")
{
  Node n1(1);
  Node n2(2);
  Node n3(1);
  REQUIRE(n1.isA<Null>()    == false);
  REQUIRE(n1.isA<Integer>() == true);
  REQUIRE(n1.isA<Boolean>() == false);
  REQUIRE(n1.isA<Float>()   == false);
  REQUIRE(n1.isA<String>()  == false);
  REQUIRE(n1.isA<Array>()   == false);
  REQUIRE(n1.isA<Object>()  == false);
  
  REQUIRE(n1 != n2);
  REQUIRE(n1 == n1);
  REQUIRE(n1 == n3);
  REQUIRE(n1.as<Integer>() == 1);

  REQUIRE(formatJson(n1) == "1");
  REQUIRE(formatJson(n2) == "2");
}

TEST_CASE("test boolea atom", "[Node]")
{
  Node f(false);
  Node t(true);
  REQUIRE(f.isA<Null>()    == false);
  REQUIRE(f.isA<Integer>() == false);
  REQUIRE(f.isA<Boolean>() == true);
  REQUIRE(f.isA<Float>()   == false);
  REQUIRE(f.isA<String>()  == false);
  REQUIRE(f.isA<Array>()   == false);
  REQUIRE(f.isA<Object>()  == false);

  REQUIRE(f != t);
  REQUIRE(f == f);
  REQUIRE(t == t);
  REQUIRE(f.as<Boolean>() == false);
  REQUIRE(t.as<Boolean>() == true);

  REQUIRE(formatJson(f) == "false");
  REQUIRE(formatJson(t) == "true");
}

TEST_CASE("test float atom", "[Node]")
{
  Node x(1.0);
  Node y(2.1);
  REQUIRE(x.isA<Null>()    == false);
  REQUIRE(x.isA<Integer>() == false);
  REQUIRE(x.isA<Boolean>() == false);
  REQUIRE(x.isA<Float>()   == true);
  REQUIRE(x.isA<String>()  == false);
  REQUIRE(x.isA<Array>()   == false);
  REQUIRE(x.isA<Object>()  == false);

  REQUIRE(x != y);
  REQUIRE(x == x);
  REQUIRE(x.as<Float>() == 1.0);

  REQUIRE(formatJson(x) == "1.0");
  REQUIRE(formatJson(y) == "2.1");
}

TEST_CASE("test string atom", "[Node]")
{
  Node s1("abc");
  Node s2("def");
  Node s3("abc");
  REQUIRE(s1.isA<Null>()    == false);
  REQUIRE(s1.isA<Integer>() == false);
  REQUIRE(s1.isA<Boolean>() == false);
  REQUIRE(s1.isA<Float>()   == false);
  REQUIRE(s1.isA<String>()  == true);
  REQUIRE(s1.isA<Array>()   == false);
  REQUIRE(s1.isA<Object>()  == false);

  REQUIRE(s1 != s2);
  REQUIRE(s1 == s1);
  REQUIRE(s1 == s3);
  REQUIRE(s1.as<String>() == "abc");

  REQUIRE(formatJson(s1) == "\"abc\"");
}

TEST_CASE("test string escape", "[Node]")
{
  REQUIRE(Node("\\t").as<String>() == "\\t");
  REQUIRE(formatJson(Node("\t")) == "\"\\t\"");
  REQUIRE(formatJson(Node("\b \n \r \t \" \\")) == "\"\\b \\n \\r \\t \\\" \\\\\"");
}

TEST_CASE("test string escape unicode", "[Node]")
{
  REQUIRE(Node(u8"\u0141 \u0143").as<String>() == u8"\u0141 \u0143");
  REQUIRE(formatJson(Node(u8"\u0141 \u0143")) == u8"\"\u0141 \u0143\"");
}

TEST_CASE("test array", "[Node]")
{
  Node array{ Null(),
              1,
              true,
              1.0,
              "abc",
              Array{Null(), 2},
              {Pair("two", 2), Pair("three", 3)}};
  Node array2 = Array{};
  Node array3 = Array();
  REQUIRE(array.isA<Null>()    == false);
  REQUIRE(array.isA<Integer>() == false);
  REQUIRE(array.isA<Boolean>() == false);
  REQUIRE(array.isA<Float>()   == false);
  REQUIRE(array.isA<String>()  == false);
  REQUIRE(array.isA<Array>()   == true);
  REQUIRE(array.isA<Object>()  == false);
  REQUIRE(array3.isA<Array>()   == true);

  REQUIRE(array != array2);
  REQUIRE(array == array);
  REQUIRE(array2 == array2);

  REQUIRE(formatJson(array) == "[null,1,true,1.0,\"abc\",[null,2],{\"two\":2,\"three\":3}]");
}

TEST_CASE("test object", "[Node]")
{
  Node object{
    Pair("null", Null()),
    Pair("integer", 1),
    Pair("boolean", true),
    Pair("float", 1.0),
    Pair("string", "value"),
    Pair("array", Array{1,2,3}),
    Pair("object", Object{{"a", 1}, {"b",2}})};

  Node object2{};
  REQUIRE(object.isA<Null>()    == false);
  REQUIRE(object.isA<Integer>() == false);
  REQUIRE(object.isA<Boolean>() == false);
  REQUIRE(object.isA<Float>()   == false);
  REQUIRE(object.isA<String>()  == false);
  REQUIRE(object.isA<Array>()   == false);
  REQUIRE(object.isA<Object>()  == true);

  REQUIRE(object != object2);
  REQUIRE(object == object);
  REQUIRE(object2 == object2);

  REQUIRE(formatJson(object) ==
          "{\"null\":null,\"integer\":1,\"boolean\":true,\"float\":1.0,\"string\":\"value\","
          "\"array\":[1,2,3],\"object\":{\"a\":1,\"b\":2}}");
  REQUIRE(formatJson(object, true) ==
          "{\n"
          "  \"null\": null,\n"
          "  \"integer\": 1,\n"
          "  \"boolean\": true,\n"
          "  \"float\": 1.0,\n"
          "  \"string\": \"value\",\n"
          "  \"array\": [\n"
          "    1,\n"
          "    2,\n"
          "    3\n"
          "  ],\n"
          "  \"object\": {\n"
          "    \"a\": 1,\n"
          "    \"b\": 2\n"
          "  }\n"
          "}");
}

TEST_CASE("object operations", "[Node]")
{
  Node obj{Pair{"name", "John"}, Pair{"age", 30}, Pair{"car", Null()}};
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":30,\"car\":null}");
  obj.as<Object>().set("lastname", "Smith");
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":30,\"car\":null,\"lastname\":\"Smith\"}");
  REQUIRE(obj.as<Object>().has("name"));
  REQUIRE(obj.as<Object>().has("lastname"));
  REQUIRE(obj.as<Object>().has("__") == false);
  REQUIRE(obj.as<Object>().modify("age", [](Node & node){ node = 31; }));
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":31,\"car\":null,\"lastname\":\"Smith\"}");
  REQUIRE(obj.as<Object>().modify("__", [](Node & node){ node = 31; }) == false);
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":31,\"car\":null,\"lastname\":\"Smith\"}");
  obj.as<Object>().forEach([](const std::string & key,
                              Node & node)
                           {
                             if(key == "age") node = 32;
                           });
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":32,\"car\":null,\"lastname\":\"Smith\"}");
  REQUIRE(obj.as<Object>().remove("__") == false);
  REQUIRE(obj.as<Object>().remove("lastname"));
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":32,\"car\":null}");
  REQUIRE(obj.as<Object>().remove([](const std::string & key,
                                     const Node & node)
                                  {
                                    return node == Null();
                                  }));
  REQUIRE(formatJson(obj) == "{\"name\":\"John\",\"age\":32}");
}

TEST_CASE("array operations", "[Node]")
{
  Node n = Array();
  REQUIRE(n.isA<Array>());
  n.as<Array>().pushBack(Integer(1));
  n.as<Array>().pushBack(Boolean(true));
  n.as<Array>().pushBack(Integer(2));
  REQUIRE(formatJson(n) == "[1,true,2]");
  n.as<Array>().remove(1);
  REQUIRE(formatJson(n) == "[1,2]");
}


TEST_CASE("update operations", "[Node]")
{
  {
    Node node = Integer(1);
    REQUIRE_THROWS(node.update({"a"}, Integer(2)));
    REQUIRE(formatJson(node) == "1");
  }
  {
    Node node = Integer(1);
    REQUIRE_THROWS(node.update({"#"}, Integer(2)));
    REQUIRE(formatJson(node) == "1");
  }
  {
    Node node = Integer(1);
    REQUIRE_THROWS(node.remove({"a"}));
  }
  {
    // { "a": 1}
    Node node = Object();
    // insert a
    REQUIRE(node.update({"a"}, Integer(1)));
    REQUIRE(formatJson(node) == "{\"a\":1}");
    // don't insert b because insert=false
    REQUIRE_FALSE(node.update({"b"}, Integer(2), false));
    REQUIRE(formatJson(node) == "{\"a\":1}");

    // update a 1->3
    REQUIRE(node.update({"a"}, Integer(3)));
    REQUIRE(formatJson(node) == "{\"a\":3}");

    // don't update subpath of a
    REQUIRE_THROWS(node.update({"a", "b", "e"}, Integer(4)));
    REQUIRE(formatJson(node) == "{\"a\":3}");

    // insert b {"e": 4}
    REQUIRE(node.update({"b", "e"}, Integer(4)));
    REQUIRE(formatJson(node) == "{\"a\":3,\"b\":{\"e\":4}}");

    // insert c [1]
    REQUIRE(node.update({"c", "#"}, Integer(4)));
    REQUIRE(formatJson(node) == "{\"a\":3,\"b\":{\"e\":4},\"c\":[4]}");

    // remove c[0]
    REQUIRE_FALSE(node.remove({"c/0"}));
    REQUIRE(node.remove({"c", "0"}));
    REQUIRE(formatJson(node) == "{\"a\":3,\"b\":{\"e\":4},\"c\":[]}");
    REQUIRE(node.remove({"c"}));
    REQUIRE(formatJson(node) == "{\"a\":3,\"b\":{\"e\":4}}");

    // remove b/e
    REQUIRE(node.remove({"b", "e"}));
    REQUIRE(formatJson(node) == "{\"a\":3,\"b\":{}}");

    // remove a
    REQUIRE(node.remove({"a"}));
    REQUIRE(formatJson(node) == "{\"b\":{}}");
  }
  {
    // [1,2]
    Node node = Array();

    // attempt to update undefined index
    REQUIRE_THROWS(node.update({"0"}, Integer(3)));

    // [1]
    REQUIRE(node.update({"#"}, Integer(1)));
    REQUIRE(formatJson(node) == "[1]");

    // [1,2]
    REQUIRE(node.update({"#"}, Integer(2)));
    REQUIRE(formatJson(node) == "[1,2]");

    // [3,2]
    REQUIRE(node.update({"0"}, Integer(3)));
    REQUIRE(formatJson(node) == "[3,2]");

    // attempt to update undefined paths
    REQUIRE_THROWS(node.update({"b"}, Integer(2)));
    REQUIRE_THROWS(node.update({"1", "a", "d"}, Integer(3)));

    // [3, {}]
    REQUIRE(node.update({"1"}, Object()));
    REQUIRE(formatJson(node) == "[3,{}]");

    // [3, {"d": {"e": 4}}]
    REQUIRE(node.update({"1", "d", "e"}, Integer(4)));
    REQUIRE(formatJson(node) == "[3,{\"d\":{\"e\":4}}]");

    // [3, {"d": {"e": 4}}, {"d":5}]
    REQUIRE(node.update({"#", "d"}, Integer(5)));
    REQUIRE(formatJson(node) == "[3,{\"d\":{\"e\":4}},{\"d\":5}]");

    // [3, {"d": {}}, {"d":5}]
    REQUIRE(node.remove({"1", "d", "e"}));
    REQUIRE(formatJson(node) == "[3,{\"d\":{}},{\"d\":5}]");

    // [3, {"d":5}]
    REQUIRE(node.remove({"1"}));
    REQUIRE(formatJson(node) == "[3,{\"d\":5}]");
  }
}
