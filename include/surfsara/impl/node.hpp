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
/////////////////////////////////////////////////////
//
// implementation details don't include directly.
//
/////////////////////////////////////////////////////
#include <sstream>

inline surfsara::ast::PathError::PathError(const std::vector<std::string> & _path,
                                           const std::string & _msg)
{
  std::string tmp;
  for(auto p : _path)
  {
    if(!tmp.empty())
    {
      tmp += '/';
    }
    tmp += p;
  }
  msg = std::string("path error: ") + tmp + std::string(" ") + _msg;
}

inline surfsara::ast::Node::Node() : value(Null()) {}

template<typename T> inline
surfsara::ast::Node::Node(T v,
                          typename std::enable_if<std::is_floating_point<T>::value>::type*)
  : value(double(v)) {}

template<typename T> inline
surfsara::ast::Node::Node(T v,
                          typename std::enable_if<std::is_integral<T>::value>::type*)
  : value(Integer(v)) {}

inline surfsara::ast::Node::Node(Null a) : value(Null()){}
inline surfsara::ast::Node::Node(Boolean a) : value(Boolean(a)){}
inline surfsara::ast::Node::Node(const Char * str) :value(String(str)) {}
inline surfsara::ast::Node::Node(String  a) : value(String(a)) {}
inline surfsara::ast::Node::Node(const Array & a) : value(Array(a)) { }
inline surfsara::ast::Node::Node(const Object & a) : value(Object(a)) {}
inline surfsara::ast::Node::Node(std::initializer_list<std::pair<String, Node>> a)
  : value(Object(a)) {}
inline surfsara::ast::Node::Node(std::initializer_list<Node> a)
  : value(Array(a)) {}

template<typename T>
bool surfsara::ast::Node::isA() const
{
  return boost::get<T>(&value) != nullptr;
}

template<typename T>
const T& surfsara::ast::Node::as() const
{
  return boost::get<T>(value);
}

template<typename T>
T& surfsara::ast::Node::as()
{
  return boost::get<T>(value);
}

inline std::string surfsara::ast::Node::typeName() const
{
  if(isA<Null>())    { return "Null"; }
  if(isA<String>())  { return "String"; }
  if(isA<Boolean>()) { return "Boolean"; }
  if(isA<Integer>()) { return "Integer"; }
  if(isA<Array>())   { return "Array"; }
  if(isA<Object>())  { return "Object"; }
  return "Unknown";
}

inline bool surfsara::ast::Node::operator==(const Node & rhs) const
{
  return value == rhs.value;
}

inline bool surfsara::ast::Node::operator!=(const Node & rhs) const
{
  return !(value == rhs.value);
}

template<typename Visitor> 
typename Visitor::result_type surfsara::ast::Node::apply_visitor(Visitor & visitor) const
{
  return boost::apply_visitor(visitor, value);
}

inline bool surfsara::ast::Node::update(const std::vector<std::string> & path,
                                        const Node & node,
                                        bool insert)
{
  return updateImpl(path, node, insert, 0);
}

inline surfsara::ast::Node surfsara::ast::Node::nodeFromPath(const std::vector<std::string> & path,
                                                             const Node & node,
                                                             std::size_t pos)
{
  if(pos < path.size())
  {
    if(path[pos] == "#")
    {
      return Array{nodeFromPath(path, node, pos+1)};
    }
    else
    {
      return Object{Pair{path[pos], nodeFromPath(path, node, pos+1)}};
    }
  }
  else
  {
    return node;
  }
}

inline bool surfsara::ast::Node::updateImpl(const std::vector<std::string> & path,
                                            const Node & node,
                                            bool insert,
                                            std::size_t pos)
{
  if(pos < path.size())
  {
    std::string key = path[pos];
    if(isA<Array>())
    {
      if(insert && key == "#")
      {
        as<Array>().pushBack(nodeFromPath(path, node, pos + 1));
        return true;
      }
      else
      {
        std::stringstream stream(key);
        std::size_t index;
        stream >> index;
        if(stream.fail() || !stream.eof())
        {
          throw PathError(path,
                          std::string("Invalid array index ") + key);
        }
        if(index < as<Array>().size())
        {
          as<Array>()[index].updateImpl(path, node, insert, pos + 1);
          return true;
        }
        else
        {
          throw PathError(path, std::string("Index out of range ") + key);
        }
      }
    }
    else if(isA<Object>())
    {
      if(insert && !as<Object>().has(key))
      {
        as<Object>().set(key, nodeFromPath(path, node, pos + 1));
        return true;
      }
      else if(as<Object>().has(key))
      {
        return as<Object>()[key].updateImpl(path, node, insert, pos + 1);
      }
      else
      {
        return false;
      }
    }
    else
    {
      throw PathError(path,
                      std::string("Could update ") + key +
                      std::string(" in object of type ") + typeName());
    }
  }
  else if(pos == path.size())
  {
    *this = node;
    return true;
  }
  else
  {
    throw PathError(path, "Could not decoode path");
  }
}


bool surfsara::ast::Node::remove(const std::vector<std::string> & path)
{
  return removeImpl(path, 0);
}

inline bool surfsara::ast::Node::removeImpl(const std::vector<std::string> & path,
                                            std::size_t pos)
{
  if(pos < path.size())
  {
    std::string key = path[pos];
    if(isA<Array>())
    {
      std::stringstream stream(key);
      std::size_t index;
      stream >> index;
      if(stream.fail() || !stream.eof())
      {
        throw PathError(path, std::string("Invalid array index ") + key);
      }
      if(index < as<Array>().size())
      {
        if(pos + 1 == path.size())
        {
          as<Array>().remove(index);
          return  true;
        }
        else
        {
          return as<Array>()[index].removeImpl(path, pos + 1);
        }
      }
      return false;
    }
    else if(isA<Object>())
    {
      if(as<Object>().has(key))
      {
        if(pos + 1 == path.size())
        {
          as<Object>().remove(path[pos]);
          return true;
        }
        else
        {
          return as<Object>()[path[pos]].removeImpl(path, pos + 1);
        }
      }
      return false;
    }
    else
    {
      throw PathError(path,
                      std::string("Could remove ") + key +
                      std::string(" from object of type ") + typeName());
    }
  }
  else
  {
    throw PathError(path, "Could not decoode path");
  }
}

