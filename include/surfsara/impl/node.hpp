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

inline surfsara::ast::Node::Node() : value(Null()) {}

template<typename T> inline
surfsara::ast::Node::Node(T v,
                          typename std::enable_if<std::is_floating_point<T>::value>::type*)
  : value(double(v)) {}

template<typename T> inline
surfsara::ast::Node::Node(T v,
                          typename std::enable_if<std::is_integral<T>::value>::type*)
  : value(Integer(v)) {}
      
inline surfsara::ast::Node::Node(Null a) : value(a){}
inline surfsara::ast::Node::Node(Boolean a) : value(Boolean(a)){}
inline surfsara::ast::Node::Node(const Char * str) :value(String(str)) {}
inline surfsara::ast::Node::Node(String  a) : value(String(a)) {}
inline surfsara::ast::Node::Node(const Array & a) : value(a) {}
inline surfsara::ast::Node::Node(const Object & a) : value(a) {}
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

