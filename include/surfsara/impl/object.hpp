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
inline surfsara::ast::Object::Object()
{
}

inline surfsara::ast::Object::Object(const std::initializer_list<std::pair<String, Node>> & l)
  : data(l)
{
  for(auto itr = data.begin(); itr != data.end(); ++itr)
  {
    lookup[itr->first] = itr;
  }
}

inline bool surfsara::ast::Object::set(const String & k, const Node & node)
{
  return setInternal(k, node);
}

inline bool surfsara::ast::Object::set(const String & k, Node && node)
{
  return setInternal(k, node);
}

inline bool surfsara::ast::Object::has(const String & v) const
{
  return lookup.find(v) != lookup.end();
}

inline surfsara::ast::Node surfsara::ast::Object::get(const String & k) const
{
  auto itr = lookup.find(k);
  if(itr == lookup.end())
  {
    /* @todo: impl. Undefined type */
    return Null();
  }
  return itr->second->second;
}

inline bool surfsara::ast::Object::modify(const String & key, std::function<void(Node & node)> lambda)
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

inline void surfsara::ast::Object::forEach(std::function<void(const String & key, Node & node)> lambda)
{
  for(auto & p : data)
  {
    lambda(p.first, p.second);
  }
}

inline void surfsara::ast::Object::forEach(std::function<void(const String & key, const Node & node)> lambda) const
{
  for(auto & p : data)
  {
    lambda(p.first, p.second);
  }
}

inline bool surfsara::ast::Object::remove(const String & key)
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

inline std::size_t surfsara::ast::Object::remove(std::function<bool(const String &, const Node & n)> predicate)
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
bool surfsara::ast::Object::setInternal(const String & key, T node)
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

inline surfsara::ast::Object::iterator surfsara::ast::Object::end()
{
  return data.end();
}

inline void surfsara::ast::Object::insert(iterator itr, const Pair & value)
{
  setInternal(value.first, value.second);
}

