#pragma once
#include <typeinfo>
#include <typeindex>
#include <tuple>
#include <string>
#include <exception>
#include <memory>
#include <iostream>
#include <type_traits>
#include <surfsara/ast.h>

namespace surfsara
{
  namespace ast
  {
    namespace detail
    {
      class Parser
      {
      public:
        enum State : int
        {
          BEGIN          = 0,
          T              = 1,
          TR             = 2,
          TRU            = 3,
          TRU_           = 4,
          F              = 5,
          FA             = 6,
          FAL            = 7,
          FALS           = 8,
          FALS_          = 9,
          N              = 10,
          NU             = 11,
          NUL            = 12,
          NUL_           = 13,
          ARRAY_BEGIN    = 14,
          ARRAY_SEP      = 15,  
          ARRAY_NEXT     = 16,
          ARRAY_END      = 17,
          OBJECT_BEGIN   = 18,
          OBJECT_SEP1    = 19,
          OBJECT_VALUE   = 20,
          OBJECT_SEP2    = 21,
          OBJECT_NEXT    = 22,
          OBJECT_END     = 23,
          DIGIT          = 30,
          FRAC_DIGIT     = 31,
          EXPONENT       = 32,
          EXPONENT_DIGIT = 33,
          STRING         = 41,
          STRING_ESC     = 42,
          STRING_END     = 43,
          STRING_UNI     = 44,
          END            = 50
        };


        typedef Node::Value Value;
        Parser(std::size_t _line=0, std::size_t _col=0)
          : line(_line), col(_col),state({BEGIN}) 
        {
        };

        const Value & getValue() const
        {
          return value.front();
        }

        State getState() const
        {
          return state.back();
        }

        std::size_t getLine() const
        {
          return line;
        }

        std::size_t getCol() const
        {
          return col;
        }

        std::tuple<std::size_t, std::size_t, Parser::State> getPos() const
        {
          return std::tuple<std::size_t, std::size_t, Parser::State>(line, col, state.back());
        }

        void parse(const std::string & str)
        {
          parseChunk(str);
          flush();
        }

        void parseChunk(const std::string & str)
        {
          parseChunk(str.c_str(), str.size());
        }

        void parseChunk(const char * str, std::size_t n)
        {
          for(std::size_t i = 0; i < n; i++)
          {
            const char * ptr = str + i;
            if(*ptr == '\n')
            {
              line++;
              col = 0;
            }
            else if(*ptr != '\r')
            {
              col++;
            }
            parseChar(*ptr);
          }
        }

        void parseChar(char ch)
        {
          switch(state.back())
          {
          case BEGIN:
          case ARRAY_NEXT:
            parseBegin(ch);
            break;
          case ARRAY_BEGIN:
            parseArrayBegin(ch);
            break;
          case ARRAY_SEP:
            parseArraySep(ch);
            break;
          case ARRAY_END:
            finalizeState(ch);
            break;
          case OBJECT_BEGIN:
            parseObjectBegin(ch);
            break;
          case OBJECT_NEXT:
            parseObjectNext(ch);
            break;
          case OBJECT_SEP1:
            if(ch == ':')
            {
              state.back() = OBJECT_VALUE;
            }
            else if(!isWhiteSpace(ch))
            {
              unexpectedCharacter(ch);
            }
            break;
          case OBJECT_VALUE:
            parseBegin(ch);
            break;
          case OBJECT_SEP2:
            parseObjectSep(ch);
            break;
          case OBJECT_END:
            finalizeState(ch);
            break;

          case T: ////////// true ///////////////
            if(ch == 'r') state.back() = TR;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case TR:
            if(ch == 'u') state.back() = TRU;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case TRU:
            if(ch == 'e') state.back() = TRU_;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case TRU_:
            finalizeState(ch);
            break;


          case F: ////////// false ///////////////
            if(ch == 'a') state.back() = FA;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case FA:
            if(ch == 'l') state.back() = FAL;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case FAL:
            if(ch == 's') state.back() = FALS;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case FALS:
            if(ch == 'e') state.back() = FALS_;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case FALS_:
            finalizeState(ch);
            break;

          case N: ////////// null ///////////////
            if(ch == 'u') state.back() = NU;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case NU:
            if(ch == 'l') state.back() = NUL;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case NUL:
            if(ch == 'l') state.back() = NUL_;
            else syntaxError("expected <string> <number> null, true, false");
            break;
          case NUL_:
            finalizeState(ch);
            break;
          case DIGIT:
            parseDigit(ch);
            break;
          case FRAC_DIGIT:
            parseFracDigit(ch);
            break;
          case EXPONENT:
            parseExponent(ch);
            break;
          case EXPONENT_DIGIT:
            parseExponentDigit(ch);
            break;
          case STRING:
            parseString(ch);
            break;
          case STRING_ESC:
            parseStringEsc(ch);
            break;
          case STRING_UNI:
            parseStringUniCode(ch);
            break;
          case STRING_END:
            finalizeState(ch);
            break;

          case END:
            if(ch != '\0' && !isWhiteSpace(ch))
            {
              unexpectedCharacter(ch);
            }
            break;
          };
        }

        void flush()
        {
          parseChar('\0');
        }

      private:
        std::size_t line;
        std::size_t col;
        
        std::vector<State> state;
        std::vector<Value> value;


        bool isWhiteSpace(char ch)
        {
          switch(ch)
          {
          case ' ':
          case '\r':
          case '\n':
          case '\t':
          case '\v':
          case '\f':
            return true;
            break;
          }
          return false;
        }

        /////////////////////////////////////////////
        //
        // helper parser function
        //
        /////////////////////////////////////////////
        /*
         * BEGIN -> T
         * BEGIN -> F
         * BEGIN -> N
         * BEGIN -> ARRAY_BEGIN
         * BEGIN -> DIGIT
         * BEGIN -> STRING
         */
        void parseBegin(char ch)
        {
          if(!isWhiteSpace(ch))
          {
            switch(ch)
            {
            case '"':
              state.push_back(STRING);
              value.push_back(Value(std::string("")));
              break;
            case 't':
              state.push_back(T);
              value.push_back(Value(true));
              break;
            case 'f':
              state.push_back(F);
              value.push_back(Value(false));
              break;
            case 'n':
              state.push_back(N);
              value.push_back(Value(Null()));
              break;
            case '[':
              state.push_back(ARRAY_BEGIN);
              value.push_back(Value(Array()));
              break;
            case '{':
              state.push_back(OBJECT_BEGIN);
              value.push_back(Value(Object()));
              break;
            case '-':
              state.push_back(DIGIT);
              value.push_back(Value(std::string("-")));
              break;
            case '+':
              state.push_back(DIGIT);
              value.push_back(Value(std::string("+")));
              break;
            case '.':
              state.push_back(FRAC_DIGIT);
              value.push_back(Value(std::string(".")));
              break;
            default:
              if(ch >= '0' && ch <= '9')
              {
                state.push_back(DIGIT);
                value.push_back(Value(std::string({ch})));
              }
              else
              {
                unexpectedCharacter(ch);
              }
            }
          }
        }

        void parseArrayBegin(char ch)
        {
          // [
          if(ch == ']')
          {
            // []
            state.back() = ARRAY_END;
          }
          else
          {
            parseBegin(ch);
          }
        }

        void parseArraySep(char ch)
        {
          if(ch == ']')
          {
            // [ <obj>, <obj>]
            state.back() = ARRAY_END;
          }
          else if(ch == ',')
          {
            // [ <obj>, <obj>  ,
            state.back() = ARRAY_NEXT;
          }
          else if(!isWhiteSpace(ch))
          {
            // [ <obj>, <obj>  X
            unexpectedCharacter(ch);
          }
        }

        void parseObjectSep(char ch)
        {
          if(ch == '}')
          {
            // { <obj>, <obj>}
            state.back() = OBJECT_END;
          }
          else if(ch == ',')
          {
            // { <obj>, <obj>  ,
            state.back() = OBJECT_NEXT;
          }
          else if(!isWhiteSpace(ch))
          {
            // { <obj>, <obj>  X
            unexpectedCharacter(ch);
          }
        }

        void parseObjectBegin(char ch)
        {
          if(ch == '}')
          {
            // {}
            state.back() = OBJECT_END;
          }
          else
          {
            if(!isWhiteSpace(ch))
            {
              if(ch == '"')
              {
                state.push_back(STRING);
                value.push_back(Value(std::string("")));
              }
              else
              {
                unexpectedCharacter(ch);
              }
            }
          }
        }

        void parseObjectNext(char ch)
        {
          if(!isWhiteSpace(ch))
          {
            if(ch == '"')
            {
              state.push_back(STRING);
              value.push_back(Value(std::string("")));
            }
            else
            {
              unexpectedCharacter(ch);
            }
          }
        }

        void finalizeState(char ch)
        {
          // <NODE>SPACE
          // <NODE>END
          // <NODE>,
          state.pop_back();
          if(state.back() == BEGIN && (ch == '\0' || isWhiteSpace(ch)))
          {
            state.back() = END;
          }
          else if(state.back() == ARRAY_BEGIN || state.back() == ARRAY_NEXT)
          {
            assert(value.size() > 1);
            (value.rbegin() + 1)->as<Array>().pushBack(Node(value.back()));
            value.pop_back();
            if(ch == ',')
            {
              // [ <obj>,
              state.back() = ARRAY_NEXT;
            }
            else if(ch == ']')
            {
              // [ <obj>]
              state.back() = ARRAY_END;
            }
            else if(isWhiteSpace(ch))
            {
              // [ <obj>
              state.back() = ARRAY_SEP;
            }
            else
            {
              unexpectedCharacter(ch);
            }
          }
          else if(state.back() == ARRAY_SEP)
          {
            parseArraySep(ch);
          }
          else if(state.back() == OBJECT_BEGIN || state.back() == OBJECT_NEXT)
          {
            if(ch == ':')
            {
              // { "key":
              state.back() = OBJECT_VALUE;
            }
            else if(isWhiteSpace(ch))
            {
              // [ "key"<spc>
              state.back() = OBJECT_SEP1;
            }
            else
            {
              unexpectedCharacter(ch);
            }
          }
          else if(state.back() == OBJECT_VALUE)
          {
            assert(value.size() > 1);
            (value.rbegin() + 2)->as<Object>().set((value.rbegin() + 1)->as<String>(), *value.rbegin());
            value.pop_back();
            value.pop_back();
            if(ch == ',')
            {
              // { <obj>,
              state.back() = OBJECT_NEXT;
            }
            else if(ch == '}')
            {
              // { <obj>}
              state.back() = OBJECT_END;
            }
            else if(isWhiteSpace(ch))
            {
              // { <obj><spc>
              state.back() = OBJECT_SEP2;
            }
            else
            {
              unexpectedCharacter(ch);
            }
          }
          else if(state.back() == END)
          {
            if(!isWhiteSpace(ch))
            {
              unexpectedCharacter(ch);
            }
          }
          else
          {
            unexpectedCharacter(ch);
          }
        }

        inline void parseDigit(char ch)
        {
          assert(!value.empty());
          assert(value.back().isA<String>());
          std::string & res(value.back().as<String>());
          
          if(ch >= '0' && ch <= '9')
          {
            res.push_back(ch);
          }
          else if(ch == '.')
          {
            res.push_back(ch);
            state.back() = FRAC_DIGIT;
          }
          else if(ch == 'e' || ch == 'E')
          {
            res.push_back(ch);
            state.back() = EXPONENT;
          }
          else
          {
            parseInteger();
            finalizeState(ch);
          }
        }

        inline void parseFracDigit(char ch)
        {
          std::string & res(value.back().as<String>());
          if(ch >= '0' && ch <= '9')
          {
            res.push_back(ch);
          }
          else if(ch == 'e' || ch == 'E')
          {
            res.push_back(ch);
            state.back() = EXPONENT;
          }
          else
          {
            parseFloat();
            finalizeState(ch);
          }
        }

        inline void parseExponent(char ch)
        {
          std::string & res(value.back().as<String>());
          if((ch >= '0' && ch <= '9') || ch == '+' || ch == '-')
          {
            res.push_back(ch);
            state.back() = EXPONENT_DIGIT;
          }
          else
          {
            unexpectedCharacter(ch);
          }
        }

        inline void parseExponentDigit(char ch)
        {
          std::string & res(value.back().as<String>());
          if(ch >= '0' && ch <= '9')
          {
            res.push_back(ch);
          }
          else
          {
            parseFloat();
            finalizeState(ch);
          }
        }

        inline void parseString(char ch)
        {
          if(ch == '\\') state.back() = STRING_ESC;
          else if(ch == '"') state.back() = STRING_END;
          else value.back().as<String>().push_back(ch);
        }

        inline void parseStringEsc(char ch)
        {
          std::string & res(value.back().as<String>());
          if(ch == 'u')
          {
            state.back() = STRING_UNI;
            value.push_back(Value(std::string("")));
          }
          else
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
            state.back() = STRING;
          }
        }

        inline void parseStringUniCode(char ch)
        {
          std::string & res(value.back().as<String>());
          if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
          {
            res.push_back(ch);
          }
          if(res.size() == 4u)
          {
            std::size_t pos = 0; 
            std::uint32_t u = std::stoul(res, &pos, 16);
            std::cout << u << std::endl;
            value.pop_back();
            state.back() = STRING;
            std::string & target(value.back().as<String>());
            target.push_back((char)(( u >>  0) & 0xff));
            target.push_back((char)(( u >>  8) & 0xff));
            target.push_back((char)(( u >> 16) & 0xff));
            target.push_back((char)(( u >> 24) & 0xff));
          }
        }

        //////////////////////////////////////
        void parseFloat()
        {
          std::string & res(value.back().as<String>());
          long double dvalue = std::stold(res);
          if(dvalue > std::numeric_limits<Float>::lowest() &&
             dvalue <= std::numeric_limits<Float>::max())
          {
            value.pop_back();
            value.push_back(Value(Float(dvalue)));
          }
          else
          {
            throw std::out_of_range(std::string("floating point value out of range ") + res);
          }
        }

        void parseInteger()
        {
          std::string & res(value.back().as<String>());
          long long ilvalue = std::stoll(res);
          if(ilvalue > std::numeric_limits<Integer>::min() &&
             ilvalue <= std::numeric_limits<Integer>::max())
          {
            value.pop_back();
            value.push_back(Value(Integer(ilvalue)));
          }
          else
          {
            throw std::out_of_range(std::string("integer value out of range ") + res);
          }
        }

        void unexpectedCharacter(char ch)
        {
          syntaxError(std::string("unexpected character '") + ch + std::string("'"));
        }

        void syntaxError(const std::string & msg)
        {
          throw std::runtime_error(msg);
        }

      };
    } // detail
  } // ast

  namespace ast
  {
    inline Node parseJson(const std::string & str)
    {
      detail::Parser p;
      p.parse(str);
      p.flush();
      return p.getValue();
    }

    template<typename I>
    inline Node parseJson(I & begin, const I & end)
    {
      detail::Parser p;
      for(I itr = begin; itr != end; ++itr)
      {
        p.parseChar(*itr);
      }
      p.flush();
      return p.getValue();
    }
  } // ast

} // surfsara
