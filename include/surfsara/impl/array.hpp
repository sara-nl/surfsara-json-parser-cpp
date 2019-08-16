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
inline surfsara::ast::Array::Array()
{
}

inline surfsara::ast::Array::Array(const std::initializer_list<Node> & l) : data(l)
{
}

inline bool surfsara::ast::Array::operator==(const Array & rhs) const
{
  return this == &rhs;
}

inline void surfsara::ast::Array::forEach(std::function<void(Node & node)> lambda)
{
  for(Node & node : data)
  {
    lambda(node);
  }
}

inline void surfsara::ast::Array::forEach(std::function<void(const Node & node)> lambda)const
{
  for(const Node & node : data)
  {
    lambda(node);
  }
}

inline std::size_t surfsara::ast::Array::size() const
{
  return data.size();
}

inline void surfsara::ast::Array::pushBack(const Node & node)
{
  data.push_back(node);
}

inline surfsara::ast::Array::iterator surfsara::ast::Array::end()
{
  return data.end();
}

inline void surfsara::ast::Array::insert(iterator itr, Node value)
{
  data.insert(itr, value);
}

inline void surfsara::ast::Array::remove(std::size_t i)
{
  data.erase(data.begin() + i);
}


inline surfsara::ast::Node & surfsara::ast::Array::operator[](std::size_t i)
{
  return data.at(i);
}

inline const surfsara::ast::Node & surfsara::ast::Array::operator[](std::size_t i) const
{
  return data.at(i);
}

inline void surfsara::ast::Array::swap(Array & rhs)
{
  data.swap(rhs.data);
}


