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
#define BOOST_SPIRIT_UNICODE
#include <boost/variant/apply_visitor.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>
#include <boost/spirit/include/qi.hpp>

namespace surfsara
{
  namespace ast
  {
    namespace detials
    {
      class JsonNodeVisitor : public boost::static_visitor<>
      {
      public:
        JsonNodeVisitor(std::ostream & _ost,
                        bool _pretty,
                        std::size_t _indent) : ost(_ost),
                                               pretty(_pretty),
                                               indent(_indent) {}

        void operator()(const Null & n) const
        {
          ost << "null";
        }

        void operator()(const Integer & i) const
        {
          ost << i;
        }

        void operator()(const Boolean & b) const
        {
          ost << (b ? "true" : "false");
        }

        void operator()(const Float & f) const
        {
          std::stringstream ss;
          ss << f;
          if(ss.str().find('.') == std::string::npos)
          {
            ost << ss.str() << ".0";
          }
          else
          {
            ost << ss.str();
          }
        }

        void operator()(const String & str) const
        {
          ost.put('"');
          typedef boost::u8_to_u32_iterator<std::string::const_iterator> iter_t;
          iter_t begin = str.begin();
          iter_t end = str.end();
          for(auto itr = begin; itr != end; ++itr)
          {
            auto ch = *itr;
            switch(ch)
            {
            case '\b':
              ost.put('\\');
              ost.put('b');
              break;
            case '\n':
              ost.put('\\');
              ost.put('n');
              break;
            case '\r':
              ost.put('\\');
              ost.put('r');
              break;
            case '\t':
              ost.put('\\');
              ost.put('t');
              break;
            case '"':
              ost.put('\\');
              ost.put('"');
              break;
            case '\\':
              ost.put('\\');
              ost.put('\\');
              break;
            case '\f':
              ost.put('\\');
              ost.put('f');
              break;
            default:
              ost << boost::spirit::to_utf8(ch);
            }
          }
          ost.put('"');
        }

        void operator()(const Array & arr) const
        {
          bool first = true;
          std::size_t locIndent = indent + 2;
          ost << "[";
          arr.forEach([&first, locIndent, this](const Node & node){
              if(first)
              {
                first = false;
              }
              else
              {
                ost << ",";
              }
              if(pretty)
              {
                putSpaceNl(ost, locIndent);
              }
              boost::apply_visitor(detials::JsonNodeVisitor(ost, pretty, indent), node);
          });
          if(pretty)
          {
            locIndent-= 2;
            putSpaceNl(ost, locIndent);
          }
          ost << "]";
        }

        void operator()(const Object & obj) const
        {
          bool first = true;
          std::size_t locIndent = indent + 2;
          ost << "{";
          obj.forEach([&first, locIndent, this](const String& key, const Node & node) {
              if(first)
              {
                first = false;
              }
              else
              {
                ost << ",";
              }
              if(pretty)
              {
                putSpaceNl(ost, locIndent);
              }
              Node n(key);
              boost::apply_visitor(detials::JsonNodeVisitor(ost, false, 0), n);
              ost << ":";
              if(pretty)
              {
                ost.put(' ');
              }
              boost::apply_visitor(detials::JsonNodeVisitor(ost, pretty, locIndent), node);
            });
          if(pretty)
          {
            locIndent-= 2;
            putSpaceNl(ost, locIndent);
          }
          ost << "}";
        }

      private:
        static void putSpaceNl(std::ostream & ost, std::size_t n)
        {
          ost.put('\n');
          for(std::size_t i = 0; i < n; i++)
          {
            ost.put(' ');
          }
        }
        std::ostream & ost;
        bool pretty;
        std::size_t indent;
      };
    }
  }
}

void surfsara::ast::formatJson(std::ostream & ost,
                               const Node & node,
                               bool pretty,
                               std::size_t indent)
{
  boost::apply_visitor(detials::JsonNodeVisitor(ost, pretty, indent), node);
}


std::string surfsara::ast::formatJson(const Node & node,
                                      bool pretty,
                                      std::size_t indent)
{
  std::stringstream ss;
  formatJson(ss, node, pretty, indent);
  return ss.str();
}

