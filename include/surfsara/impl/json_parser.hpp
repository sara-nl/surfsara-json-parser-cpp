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
/////////////////////////////////////////////////////
//
// implementation details, don't include directly.
//
/////////////////////////////////////////////////////
#pragma once
#include <stdexcept>
#define BOOST_SPIRIT_UNICODE
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/locale.hpp>
#include <boost/version.hpp>
#include <functional>

namespace surfsara
{
  namespace ast
  {
    namespace details
    {
      struct UnicodeDecoder
      {
        void operator()(String & res, uint32_t ch) const
        {
          res+= boost::locale::conv::utf_to_utf<char>(&ch, &ch+1);
        }
      };

      struct Decoder
      {
        void operator()(String & res, char ch) const
        {
          switch(ch)
          {
          case '"': res.push_back('"');   break;
          case '\\': res.push_back('\\'); break;
          case '/': res.push_back('/');   break;
          case 'b': res.push_back(0x08);  break;
          case 'f': res.push_back(0x0c);  break;
          case 'n': res.push_back('\n');  break;
          case 'r': res.push_back('\r');  break;
          case 't': res.push_back('\t');  break;
          }
        }
      };
    }

    inline Node parseJson(const String & str)
    {
      std::string::const_iterator itr = str.begin();
      std::string::const_iterator end = str.end();
      return parseJson(itr, end);
    }

    template<typename I>
    Node parseJson(I & itr, const I & end)
    {
      using boost::spirit::qi::lexeme;
      using boost::spirit::qi::double_;
      using boost::spirit::qi::int_;
      using boost::spirit::qi::bool_;
      using boost::spirit::ascii::char_;
      using boost::spirit::qi::_val_type;
      using boost::spirit::ascii::space_type;
      boost::spirit::qi::lit_type                        lit;
      boost::spirit::qi::attr_type                       attr;
      boost::spirit::qi::bool_type                       boolValue;
      boost::spirit::qi::_r1_type                        _r1;
      boost::spirit::qi::_1_type                         _1;
      boost::spirit::qi::_val_type                       _val;
      boost::spirit::qi::uint_parser<uint32_t, 16, 4, 4> hexParser;
      boost::spirit::qi::rule<I, Node(),  space_type>    value;
      boost::spirit::qi::rule<I, Array(), space_type >   array;
      boost::spirit::qi::rule<I, Object(), space_type >  object;
      boost::spirit::qi::rule<I, Pair(),  space_type >   keyValuePair;
      boost::spirit::qi::rule<I, String()>               str;
      boost::spirit::qi::rule<I, void(String&)>          unicode;
      boost::phoenix::function<details::Decoder>         decoder;
      boost::spirit::qi::symbols<char,char>              escaped;
      escaped.add("\\\\" , '\\')
                 ("\\\"" , '"' )
                 ("\\n"  , '\n')
                 ("\\r"  , '\r')
                 ("\\b"  , '\b')
                 ("\\f"  , '\f')
                 ("\\t"  , '\t')
                 ("\\u"  , '\0');

#if BOOST_VERSION < 106500
      str = lexeme[lit('"') >>
                   *((escaped | char_) - '"') >>
                   lit('"')];

#else
      boost::phoenix::function<details::UnicodeDecoder>  unicodeDecoder;
      unicode = ("\\u" > hexParser)[unicodeDecoder(_r1, _1)];
      str = lexeme[lit('"') >>
                   *( unicode(_val) | ((escaped | char_) - '"')[ _val += _1]) >>
                   lit('"')];
#endif
      keyValuePair = (str >> ':' >> value)
        [boost::spirit::qi::_val = boost::phoenix::construct<Pair>(boost::spirit::qi::_1,
                                                                   boost::spirit::qi::_2)];
      array = lit('[') >> -(value % ',') >> lit(']');
      object = lit('{') >> -(keyValuePair % ',') >> lit('}');
      value =
        (lit("null") >> attr(Null())) |
        boolValue |
        str |
        lexeme[int_ >> !char_(".eE")] |
        double_ |
        array |
        object;
      Node node;
      bool r = boost::spirit::qi::phrase_parse(itr,
                                               end,
                                               value,
                                               boost::spirit::ascii::space,
                                               node);
      if(!r)
      {
        throw std::runtime_error("Failed to parse JSON string");
      }
      if(itr != end)
      {
        throw std::runtime_error(std::string("Failed to parse JSON string close to:"));
      }
      return node;
    }
  }
}





