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
#pragma once
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace surfsara
{
  namespace ast
  {
    struct Null
    {
      inline bool operator==(const Null & rhs) const{ return true; }
    };

    typedef int64_t Integer;
    class Node;

    class Array
    {
    public:
      Array(const std::initializer_list<Node> & l) : data(l) {}
      inline bool operator==(const Array & rhs) const{ return this == &rhs; }
    private:
      std::vector<Node> data;
    };

    class Object
    {
    public:
      Object(const std::initializer_list<std::pair<std::string, Node>> & l);
      inline bool set(const std::string & k, const Node & node);
      inline bool set(const std::string & k, Node && node);
      inline bool has(const std::string & v);
      inline bool modify(const std::string & , std::function<void(Node & node)> lambda);
      inline void forEach(std::function<void(const std::string &, Node & node)> lambda);
      inline void forEach(std::function<void(const std::string &, const Node & node)> lambda) const;
      inline bool remove(const std::string &);
      inline std::size_t remove(std::function<bool(const std::string &, const Node & n)> predicate);
      inline bool operator==(const Object & rhs)const{ return this == &rhs; }
    private:
      template<typename T>
      inline bool setInternal(const std::string & k, T node);
      std::list<std::pair<std::string, Node>> data;
      using iterator = std::list<std::pair<std::string, Node>>::iterator;
      std::unordered_map<std::string, iterator> lookup;
    };

    class Node
    {
    public:
      Node() : value(Null()) {}
      template<typename T>
      Node(T v, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr) : value(double(v)) {}
      
      template<typename T>
      Node(T v, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) : value(Integer(v)) {}
      
      Node(Null a) : value(a){}
      Node(bool a) : value(a){}
      Node(std::string  a) : value(a) {}
      Node(const Array & a) : value(a) {}
      Node(const Object & a) : value(a) {}
      Node(std::initializer_list<std::pair<std::string, Node>> a) : value(Object(a)) {}
      Node(std::initializer_list<Node> a) : value(Array(a)) {}

      template<typename T>
      bool isA() const
      {
        return boost::get<T>(&value) != nullptr;
      }

      template<typename T>
      const T& as() const
      {
        return boost::get<T>(value);
      }

      inline bool operator==(const Node & rhs) const
      {
        return value == rhs.value;
      }

      inline bool operator!=(const Node & rhs) const
      {
        return !(value == rhs.value);
      }

    private:
      typedef boost::variant<Integer,
                             double,
                             bool,
                             Null,
                             std::string,
                             Array,
                             Object> Value;
      Value value;
    };

    inline std::pair<std::string, Node> Pair(const std::string & k, const Node & node)
    {
      return std::make_pair(k, node);
    }
  } // ns ast
} // ns surfsara

////////////////////////////////////////////////////////////////////////////////
//
// Implementation
//
////////////////////////////////////////////////////////////////////////////////
inline surfsara::ast::Object::Object(const std::initializer_list<std::pair<std::string, Node>> & l) : data(l)
{
  for(auto itr = data.begin(); itr != data.end(); ++itr)
  {
    lookup[itr->first] = itr;
  }
}

inline bool surfsara::ast::Object::set(const std::string & k, const Node & node)
{
  return setInternal(k, node);
}

inline bool surfsara::ast::Object::set(const std::string & k, Node && node)
{
  return setInternal(k, node);
}

inline bool surfsara::ast::Object::has(const std::string & v)
{
  return lookup.find(v) != lookup.end();
}

inline bool surfsara::ast::Object::modify(const std::string & key, std::function<void(Node & node)> lambda)
{
  auto itr = lookup.find(key);
  if(itr == lookup.end())
  {
    return false;
  }
  else
  {
    lambda(itr->second->second);
    return true;
  }
}

inline void surfsara::ast::Object::forEach(std::function<void(const std::string & key, Node & node)> lambda)
{
  for(auto p : data)
  {
    lambda(p.first, p.second);
  }
}

inline void surfsara::ast::Object::forEach(std::function<void(const std::string & key, const Node & node)> lambda) const
{
  for(auto p : data)
  {
    lambda(p.first, p.second);
  }
}

inline bool surfsara::ast::Object::remove(const std::string & key)
{
  auto itr = lookup.find(key);
  if(itr == lookup.end())
  {
    return false;
  }
  else
  {
    data.erase(itr->second);
    lookup.erase(key);
    return true;
  }
}

inline std::size_t surfsara::ast::Object::remove(std::function<bool(const std::string &, const Node & n)> predicate)
{
  std::size_t n = 0;
  for(auto itr = data.begin(); itr != data.end();)
  {
    if(predicate(itr->first, itr->second))
    {
      itr = data.erase(itr);
      n++;
    }
    else
    {
      ++itr;
    }
  }
  return n;
}


template<typename T>
bool surfsara::ast::Object::setInternal(const std::string & key, T node)
{
  auto itr = lookup.find(key);
  if(itr == lookup.end())
  {
    auto p = data.insert(data.end(), std::make_pair(key, node));
    lookup[key] = p;
    return true;
  }
  else
  {
    *itr->second = std::make_pair(key, node);
    return false;
  }
}
