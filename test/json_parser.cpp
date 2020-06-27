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
#include <surfsara/json_parser.h>
#include <surfsara/json_format.h>
#include <boost/version.hpp>

using namespace surfsara::ast;

TEST_CASE("parse null", "[JsonParser]")
{
  auto node = parseJson("null");
  REQUIRE(node.isA<Null>());
}

TEST_CASE("parse integer", "[JsonParser]")
{
  auto node = parseJson("1");
  REQUIRE(node.isA<Integer>());
  REQUIRE(node.as<Integer>() == 1);
}

TEST_CASE("failed integer parsing throws", "[JsonParser]")
{
  REQUIRE_THROWS(parseJson("1xxxx"));
  REQUIRE_THROWS(parseJson("xxx"));
}

TEST_CASE("parse boolean", "[JsonParser]")
{
  auto t = parseJson("true");
  auto f = parseJson("false");
  REQUIRE(t.isA<Boolean>());
  REQUIRE(f.isA<Boolean>());
  REQUIRE(t.as<Boolean>() == true);
  REQUIRE(f.as<Boolean>() == false);
}

TEST_CASE("parse float", "[JsonParser]")
{
  auto f = parseJson("1.0");
  REQUIRE(f.isA<Float>());
  REQUIRE(f.as<Float>() == 1.0);
}

TEST_CASE("parse string", "[JsonParser]")
{
  REQUIRE(parseJson("\"\"").isA<String>());
  REQUIRE(parseJson("\"\"").as<String>() == "");
  REQUIRE(parseJson("\"abc\"").as<String>() == "abc");
  REQUIRE(parseJson("\"\\b\"").as<String>() == "\b");
  REQUIRE(parseJson("\"\\n\"").as<String>() == "\n");
  REQUIRE(parseJson("\"\\r\"").as<String>() == "\r");
  REQUIRE(parseJson("\"\\t\"").as<String>() == "\t");
  REQUIRE(parseJson("\" \\\\ \"").as<String>() == " \\ ");
  REQUIRE(parseJson("\" \\\" \"").as<String>() == " \" ");
  REQUIRE(parseJson("\" \\n \\r \\t \\\" \\\\ \"").as<String>() == " \n \r \t \" \\ ");
#ifdef NO_BOOST_PARSING
  REQUIRE(formatJson(parseJson("\"\\u0141 \\u0143\"")) == "\"\u0141 \u0143\"");
#else
#if BOOST_VERSION >= 106500
  REQUIRE(formatJson(parseJson("\"\\u0141 \\u0143\"")) == "\"\u0141 \u0143\"");
#endif
#endif // boost
}

TEST_CASE("parse array", "[JsonParser]")
{
  std::string array = "[null,1,true,1.0,\"abc\",[null,2],{\"two\":2,\"three\":3}]";
  REQUIRE(parseJson(array).isA<Array>());
  REQUIRE(formatJson(parseJson(array)) == array);
}

TEST_CASE("parse object", "[JsonParser]")
{
  std::string json =
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
    "}";
  std::string result =
    "{\"null\":null,\"integer\":1,\"boolean\":true,\"float\":1.0,\"string\":\"value\","
    "\"array\":[1,2,3],\"object\":{\"a\":1,\"b\":2}}";
  REQUIRE(parseJson(json).isA<Object>());
  REQUIRE(formatJson(parseJson(json)) == result);
}
