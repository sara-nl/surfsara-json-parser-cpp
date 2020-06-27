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
#include <surfsara/impl/json_parser_no_boost.hpp>
#include <iostream>

//using namespace surfsara::ast;
using Parser = surfsara::ast::detail::Parser;
using String = surfsara::ast::String;
using Boolean = surfsara::ast::Boolean;
using Null = surfsara::ast::Null;
using Integer = surfsara::ast::Integer;
using Float = surfsara::ast::Float;
using Array = surfsara::ast::Array;
using Object = surfsara::ast::Object;

std::tuple<std::size_t, std::size_t, Parser::State> P(std::size_t line, std::size_t col, Parser::State s)
{
  return std::tuple<std::size_t, std::size_t, Parser::State>(line, col, s);
}

std::ostream & operator<<(std::ostream & ost, const std::tuple<std::size_t, std::size_t, Parser::State> & pos)
{
  ost << "(" << std::get<0>(pos) << ":" << std::get<1>(pos) << " " << std::get<2>(pos) << ")";
  return ost;
}

//////////////////////////////////////////////////
//
// Boolean
//
//////////////////////////////////////////////////
TEST_CASE("parse_boolean", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk("");
    REQUIRE(p.getPos() == P(0u, 0u, Parser::BEGIN));
    p.parseChunk("  ");
    REQUIRE(p.getPos() == P(0u, 2u, Parser::BEGIN));
    p.parseChunk("\n ");
    REQUIRE(p.getPos() == P(1u, 1u, Parser::BEGIN));
    p.parseChunk(" true");
    REQUIRE(p.getPos() == P(1u, 6u, Parser::TRU_));
    p.parseChunk(" ");
    REQUIRE(p.getPos() == P(1u, 7u, Parser::END));
    p.flush();
  }

  {
    Parser p;
    p.parseChunk(" false");
    REQUIRE(p.getPos() == P(0u, 6u, Parser::FALS_));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 6u, Parser::END));
  }
  {
    Parser p;
    p.parse(" true");
    REQUIRE(p.getPos() == P(0u, 5u, Parser::END));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" faxe  "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" false,  "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" true ,  "));
  }
}

//////////////////////////////////////////////////
//
// Null
//
//////////////////////////////////////////////////
TEST_CASE("parse_null", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk(" null  ");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::END));
  }
  {
    Parser p;
    p.parseChunk(" null");
    REQUIRE(p.getPos() == P(0u, 5u, Parser::NUL_));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 5u, Parser::END));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" nu l  "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" nul,  "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" null,  "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" null ,  "));
  }
}

//////////////////////////////////////////////////
//
// String
//
//////////////////////////////////////////////////
TEST_CASE("parse_string", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk("");
    REQUIRE(p.getPos() == P(0u, 0u, Parser::BEGIN));
    p.parseChunk("  ");
    REQUIRE(p.getPos() == P(0u, 2u, Parser::BEGIN));
    p.parseChunk("\"");
    REQUIRE(p.getPos() == P(0u, 3u, Parser::STRING));
    p.parseChunk("abc");
    REQUIRE(p.getPos() == P(0u, 6u, Parser::STRING));
    p.parseChunk("\"");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::STRING_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 7u, Parser::END));
    REQUIRE_THROWS(p.parseChunk(" \""));
    REQUIRE(p.getValue().isA<String>());
    REQUIRE(p.getValue().as<String>() == "abc");
  }
  std::vector<std::pair<std::string, std::string>> cases({
      {"\"\"", ""},
      {"\"abc\"", "abc"},
      {"\"\\b\"", "\b"},
      {"\"\\n\"", "\n"},
      {"\"\\r\"", "\r"},
      {"\"\\t\"", "\t"},
      {"\" \\\\ \"", " \\ "},
      {"\" \\\" \"", " \" "},
      {"\" \\n \\r \\t \\\" \\\\ \"", " \n \r \t \" \\ "}
    });
  //@todo: implement unicode support
  //{"\"\\u0141 \\u0143\"", "\"\u0141 \u0143\""}
  for(const auto & c : cases)
  {
    Parser p;
    p.parse(c.first);
    REQUIRE(p.getValue().isA<String>());
    REQUIRE(p.getValue().as<String>() == c.second);
  }
}

//////////////////////////////////////////////////
//
// Integer
//
//////////////////////////////////////////////////
TEST_CASE("parse_integer", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk("+");
    REQUIRE(p.getPos() == P(0u, 1u, Parser::DIGIT));
  }
  {
    Parser p;
    p.parseChunk("-");
    REQUIRE(p.getPos() == P(0u, 1u, Parser::DIGIT));
  }
  {
    Parser p;
    p.parseChunk("0");
    REQUIRE(p.getPos() == P(0u, 1u, Parser::DIGIT));
    p.parseChunk("123456789");
    REQUIRE(p.getPos() == P(0u, 10u, Parser::DIGIT));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 10u, Parser::END));
    REQUIRE(p.getValue().isA<Integer>());
    REQUIRE(p.getValue().as<Integer>() == 123456789);
  }
  {
    Parser p;
    p.parseChunk(".");
    REQUIRE(p.getPos() == P(0u, 1u, Parser::FRAC_DIGIT));
  }

}

TEST_CASE("parse_integer_exceptions", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk("234342320398420243802983409238234234234234234");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("+234342320398420243802983409238234234234234444");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("-234342320398420243802983409238234234234234234");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk("-234abc"));
  }
}

//////////////////////////////////////////////////
//
// Floating point
//
//////////////////////////////////////////////////
TEST_CASE("parse_floating_point", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk(".");
    REQUIRE(p.getPos() == P(0u, 1u, Parser::FRAC_DIGIT));
  }
  {
    Parser p;
    p.parseChunk(".0");
    REQUIRE(p.getPos() == P(0u, 2u, Parser::FRAC_DIGIT));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 2u, Parser::END));
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(0.0));
  }
  {
    Parser p;
    p.parseChunk("123.");
    REQUIRE(p.getPos() == P(0u, 4u, Parser::FRAC_DIGIT));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 4u, Parser::END));
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(123.));
  }
  {
    Parser p;
    p.parseChunk("-123.456");
    REQUIRE(p.getPos() == P(0u, 8u, Parser::FRAC_DIGIT));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 8u, Parser::END));
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(-123.456));
  }
  {
    Parser p;
    p.parseChunk("-123e");
    REQUIRE(p.getPos() == P(0u, 5u, Parser::EXPONENT));
  }
  {
    Parser p;
    p.parseChunk("-123.e");
    REQUIRE(p.getPos() == P(0u, 6u, Parser::EXPONENT));
  }
  {
    Parser p;
    p.parseChunk("123.1e");
    REQUIRE(p.getPos() == P(0u, 6u, Parser::EXPONENT));
  }
  {
    Parser p;
    p.parseChunk("123.1e-");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::EXPONENT_DIGIT));
  }
  {
    Parser p;
    p.parseChunk("123.1e+");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::EXPONENT_DIGIT));
  }
  {
    Parser p;
    p.parseChunk("123.1e10");
    REQUIRE(p.getPos() == P(0u, 8u, Parser::EXPONENT_DIGIT));
    p.flush();
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(123.1e10));
  }
  {
    Parser p;
    p.parse("123.1e-10");
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(123.1e-10));
  }
  {
    Parser p;
    p.parse("-123.1e+10");
    REQUIRE(p.getValue().isA<Float>());
    REQUIRE(p.getValue().as<Float>() == Approx(-123.1e10));
  }
}


TEST_CASE("parse_floating_point_exceptions", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk(".");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("1.0e10000");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("-1.0e10000");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("-1.0e-10000");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk("1.0e-10000");
    REQUIRE_THROWS(p.flush());
  }
}

//////////////////////////////////////////////////
//
// Array
//
//////////////////////////////////////////////////
TEST_CASE("parse_array", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk(" [  ]");
    REQUIRE(p.getPos() == P(0u, 5u, Parser::ARRAY_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 5u, Parser::END));
    REQUIRE_THROWS(p.parseChunk(" \""));
    REQUIRE(p.getValue().isA<Array>());
    REQUIRE(p.getValue().as<Array>().size() == 0u);
  }
  {
    Parser p;
    p.parseChunk("[true]");
    REQUIRE(p.getPos() == P(0u, 6u, Parser::ARRAY_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 6u, Parser::END));
    REQUIRE_THROWS(p.parseChunk(" \""));
    REQUIRE(p.getValue().isA<Array>());
    REQUIRE(p.getValue().as<Array>().size() == 1u);
    REQUIRE(p.getValue().as<Array>()[0].isA<Boolean>());
    REQUIRE(p.getValue().as<Array>()[0].as<Boolean>() == true);

  }
  {
    Parser p;
    p.parseChunk(" [ true ] ");
    REQUIRE(p.getPos() == P(0u, 10u, Parser::END));
    REQUIRE(p.getValue().as<Array>().size() == 1u);
    REQUIRE(p.getValue().as<Array>()[0].isA<Boolean>());
    REQUIRE(p.getValue().as<Array>()[0].as<Boolean>() == true);
  }
  {
    Parser p;
    p.parseChunk(" [  true");
    REQUIRE(p.getPos() == P(0u, 8u, Parser::TRU_));
    p.parseChunk(" ");
    REQUIRE(p.getPos() == P(0u, 9u, Parser::ARRAY_SEP));
    p.parseChunk(",");
    REQUIRE(p.getPos() == P(0u, 10u, Parser::ARRAY_NEXT));
    p.parseChunk("f");
    REQUIRE(p.getPos() == P(0u, 11u, Parser::F));
    p.parseChunk("alse");
    REQUIRE(p.getPos() == P(0u, 15u, Parser::FALS_));
    p.parseChunk(",");
    REQUIRE(p.getPos() == P(0u, 16u, Parser::ARRAY_NEXT));
    p.parseChunk("tr");
    REQUIRE(p.getPos() == P(0u, 18u, Parser::TR));
    p.parseChunk("ue]");
    REQUIRE(p.getPos() == P(0u, 21u, Parser::ARRAY_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 21u, Parser::END));

    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 3);
    REQUIRE(arr[0].isA<Boolean>());
    REQUIRE(arr[0].as<Boolean>() == true);
    REQUIRE(arr[1].isA<Boolean>());
    REQUIRE(arr[1].as<Boolean>() == false);
    REQUIRE(arr[2].isA<Boolean>());
    REQUIRE(arr[2].as<Boolean>() == true);
  }
  {
    Parser p;
    p.parseChunk(" [  true, false , true]  ");
    REQUIRE(p.getPos() == P(0u, 25u, Parser::END));
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 3);
    REQUIRE(arr[0].isA<Boolean>());
    REQUIRE(arr[0].as<Boolean>() == true);
    REQUIRE(arr[1].isA<Boolean>());
    REQUIRE(arr[1].as<Boolean>() == false);
    REQUIRE(arr[2].isA<Boolean>());
    REQUIRE(arr[2].as<Boolean>() == true);
  }
  {
    Parser p;
    p.parseChunk(" [  true, false ,[], true]  ");
    REQUIRE(p.getPos() == P(0u, 28u, Parser::END));

    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 4);
    REQUIRE(arr[0].isA<Boolean>());
    REQUIRE(arr[0].as<Boolean>() == true);
    REQUIRE(arr[1].isA<Boolean>());
    REQUIRE(arr[1].as<Boolean>() == false);
    REQUIRE(arr[2].isA<Array>());
    REQUIRE(arr[2].as<Array>().size() == 0u);
    REQUIRE(arr[3].isA<Boolean>());
    REQUIRE(arr[3].as<Boolean>() == true);

  }
  {
    Parser p;
    p.parseChunk(" [  true, false ,[true, false ], true]  ");
    REQUIRE(p.getPos() == P(0u, 40u, Parser::END));

    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 4);
    REQUIRE(arr[0].isA<Boolean>());
    REQUIRE(arr[0].as<Boolean>() == true);
    REQUIRE(arr[1].isA<Boolean>());
    REQUIRE(arr[1].as<Boolean>() == false);
    REQUIRE(arr[2].isA<Array>());
    REQUIRE(arr[2].as<Array>().size() == 2u);
    const Array & arr2(p.getValue().as<Array>());
    REQUIRE(arr2[0].isA<Boolean>());
    REQUIRE(arr2[0].as<Boolean>() == true);
    REQUIRE(arr2[1].isA<Boolean>());
    REQUIRE(arr2[1].as<Boolean>() == false);
    REQUIRE(arr[3].isA<Boolean>());
    REQUIRE(arr[3].as<Boolean>() == true);
  }
  {
    Parser p;
    p.parse(" [[],[], [[],[]]]");
    REQUIRE(p.getPos() == P(0u, 17u, Parser::END));
    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 3);
    REQUIRE(arr[0].isA<Array>());
    const Array & arr0(arr[0].as<Array>());
    REQUIRE(arr0.size() == 0u);

    REQUIRE(arr[1].isA<Array>());
    const Array & arr1(arr[1].as<Array>());
    REQUIRE(arr1.size() == 0u);

    REQUIRE(arr[2].isA<Array>());
    const Array & arr2(arr[2].as<Array>());
    REQUIRE(arr2.size() == 2u);

    REQUIRE(arr2[0].isA<Array>());
    REQUIRE(arr2[0].as<Array>().size() == 0u);
    REQUIRE(arr2[1].isA<Array>());
    REQUIRE(arr2[1].as<Array>().size() == 0u);
  }
  {
    Parser p;
    p.parse("[ \"abc\", true, false, null, [] ]");
    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 5u);
    REQUIRE(arr[0].isA<String>());
    REQUIRE(arr[0].as<String>() == "abc");
    REQUIRE(arr[1].isA<Boolean>());
    REQUIRE(arr[1].as<Boolean>() == true);
    REQUIRE(arr[2].isA<Boolean>());
    REQUIRE(arr[2].as<Boolean>() == false);
    REQUIRE(arr[3].isA<Null>());
    REQUIRE(arr[4].isA<Array>());
    REQUIRE(arr[4].as<Array>().size() == 0u);
  }
}

TEST_CASE("parse_array_of_numbers", "[JsonParser]")
{
  {
    Parser p;
    p.parse("[1 ]");
    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 1u);
    REQUIRE(arr[0].isA<Integer>());
    REQUIRE(arr[0].as<Integer>() == 1);
  }
  {
    Parser p;
    p.parseChunk("[1,2");
    REQUIRE(p.getPos() == P(0u, 4u, Parser::DIGIT));
    p.parseChunk("]");
    REQUIRE(p.getPos() == P(0u, 5u, Parser::ARRAY_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 5u, Parser::END));
    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 2u);
    REQUIRE(arr[0].isA<Integer>());
    REQUIRE(arr[0].as<Integer>() == 1);
    REQUIRE(arr.size() == 2u);
    REQUIRE(arr[1].isA<Integer>());
    REQUIRE(arr[1].as<Integer>() == 2);
  }
  {
    Parser p;
    p.parse("[ 1, 1.2, 0, -1e4 ]");
    REQUIRE(p.getValue().isA<Array>());
    const Array & arr(p.getValue().as<Array>());
    REQUIRE(arr.size() == 4u);
    REQUIRE(arr[0].isA<Integer>());
    REQUIRE(arr[0].as<Integer>() == 1);
    REQUIRE(arr[1].isA<Float>());
    REQUIRE(arr[1].as<Float>() == Approx(1.2));
    REQUIRE(arr[2].isA<Integer>());
    REQUIRE(arr[2].as<Integer>() == 0);
    REQUIRE(arr[3].isA<Float>());
    REQUIRE(arr[3].as<Float>() == Approx(-1e4));
  }
}

TEST_CASE("parse_array_exceptions", "[JsonParser]")
{
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" ] "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" [] ] "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" , "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" [,] "));
  }
  {
    Parser p;
    p.parseChunk(" [ ");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk("[x]"));
  }
  {
    Parser p;
    p.parseChunk(" [ true");
    REQUIRE_THROWS(p.flush());
  }
  {
    Parser p;
    p.parseChunk(" [ true ");
    REQUIRE_THROWS(p.flush());
  }
}

//////////////////////////////////////////////////
//
// Object
//
//////////////////////////////////////////////////
TEST_CASE("parse_object_with_one_element", "[JsonParser]")
{
  {
    Parser p;
    p.parseChunk(" {");
    REQUIRE(p.getPos() == P(0u, 2u, Parser::OBJECT_BEGIN));
    p.parseChunk("\n ");
    REQUIRE(p.getPos() == P(1u, 1u, Parser::OBJECT_BEGIN));
    p.parseChunk(" } ");
    p.flush();
    REQUIRE(p.getPos() == P(1u, 4u, Parser::END));
    REQUIRE(p.getValue().isA<Object>());
    const Object & obj(p.getValue().as<Object>());
    REQUIRE(obj.empty());
  }
  {
    Parser p;
    p.parseChunk("{\"abc\" ");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::OBJECT_SEP1));
    p.parseChunk(":");
    REQUIRE(p.getPos() == P(0u, 8u, Parser::OBJECT_VALUE));
    p.parseChunk("\n");
    REQUIRE(p.getPos() == P(1u, 0u, Parser::OBJECT_VALUE));
    p.parseChunk("\"test");
    REQUIRE(p.getPos() == P(1u, 5u, Parser::STRING));
    p.parseChunk("\"");
    REQUIRE(p.getPos() == P(1u, 6u, Parser::STRING_END));
    p.parseChunk("}");
    REQUIRE(p.getPos() == P(1u, 7u, Parser::OBJECT_END));
    p.flush();
    REQUIRE(p.getPos() == P(1u, 7u, Parser::END));
    REQUIRE(p.getValue().isA<Object>());
    const Object & obj(p.getValue().as<Object>());
    REQUIRE(obj.size() == 1u);
    REQUIRE(obj.has("abc"));
    REQUIRE(obj["abc"].isA<String>());
    REQUIRE(obj["abc"].as<String>() == "test");
  }
  {
    Parser p;
    p.parseChunk("{\"t\": true,");
    REQUIRE(p.getPos() == P(0u, 11u, Parser::OBJECT_NEXT));
    p.parseChunk("\n\"f\": false}");
    REQUIRE(p.getPos() == P(1u, 11u, Parser::OBJECT_END));
    p.flush();
    REQUIRE(p.getPos() == P(1u, 11u, Parser::END));
    REQUIRE(p.getValue().isA<Object>());
    const Object & obj(p.getValue().as<Object>());
    REQUIRE(obj.size() == 2u);
    REQUIRE(obj.has("t"));
    REQUIRE(obj.has("f"));
    REQUIRE(obj["t"].isA<Boolean>());
    REQUIRE(obj["t"].as<Boolean>());
    REQUIRE(obj["f"].isA<Boolean>());
    REQUIRE_FALSE(obj["f"].as<Boolean>());
  }
  {
    Parser p;
    p.parseChunk("{\"s\":1}");
    REQUIRE(p.getPos() == P(0u, 7u, Parser::OBJECT_END));
    p.flush();
    REQUIRE(p.getPos() == P(0u, 7u, Parser::END));
    REQUIRE(p.getValue().isA<Object>());
    const Object & obj(p.getValue().as<Object>());
    REQUIRE(obj.size() == 1u);
    REQUIRE(obj.has("s"));
    REQUIRE(obj["s"].isA<Integer>());
    REQUIRE(obj["s"].as<Integer>() == 1);
  }
}

TEST_CASE("parse_object_with_more_elements", "[JsonParser]")
{
  {
    Parser p;
    p.parse("{\"a\": 1 , \"b\": 2e10, \"c\": 10, \"d\": true, \"e\": \"str\"}");
    REQUIRE(p.getValue().isA<Object>());
    const Object & obj(p.getValue().as<Object>());
    REQUIRE(obj.size() == 5u);
    REQUIRE(obj.has("a"));
    REQUIRE(obj["a"].isA<Integer>());
    REQUIRE(obj["a"].as<Integer>() == 1);
    REQUIRE(obj.has("b"));
    REQUIRE(obj["b"].isA<Float>());
    REQUIRE(obj["b"].as<Float>() == Approx(2e10));
    REQUIRE(obj.has("c"));
    REQUIRE(obj["c"].isA<Integer>());
    REQUIRE(obj["c"].as<Integer>() == 10);
    REQUIRE(obj.has("d"));
    REQUIRE(obj["d"].isA<Boolean>());
    REQUIRE(obj["d"].as<Boolean>());
    REQUIRE(obj.has("e"));
    REQUIRE(obj["e"].isA<String>());
    REQUIRE(obj["e"].as<String>() == "str");
  }
}

TEST_CASE("parse_object_exceptions", "[JsonParser]")
{
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" } "));
  }
  {
    Parser p;
    REQUIRE_THROWS(p.parseChunk(" { \"abc\" } "));
  }
}
