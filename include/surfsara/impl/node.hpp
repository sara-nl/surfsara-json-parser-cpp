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
#include <boost/algorithm/string.hpp>

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

///////////////////////////////////////////////////////////////////////////////
//
// Node::Value
//
///////////////////////////////////////////////////////////////////////////////
inline surfsara::ast::Node::Value::Value(const Null & n) : typeIndex(std::type_index(typeid(Null)))
{
}

inline surfsara::ast::Node::Value::Value(const Undefined & u) : typeIndex(std::type_index(typeid(Undefined)))
{
}

inline surfsara::ast::Node::Value::Value(const Boolean & b) : typeIndex(std::type_index(typeid(Boolean)))
{
  v.booleanValue = b;
}

inline surfsara::ast::Node::Value::Value(const Integer & i) : typeIndex(std::type_index(typeid(Integer)))
{
  v.integerValue = i;
}

inline surfsara::ast::Node::Value::Value(const Float & f) : typeIndex(std::type_index(typeid(Float)))
{
  v.floatValue = f;
}

inline surfsara::ast::Node::Value::Value(const String & s) : typeIndex(std::type_index(typeid(String)))
{
  v.stringValue = new String(s);
}

inline surfsara::ast::Node::Value::Value(const Array & a) : typeIndex(std::type_index(typeid(Array)))
{
  v.arrayValue = new Array(a);
}

inline surfsara::ast::Node::Value::Value(Array && a) : typeIndex(std::type_index(typeid(Array)))
{
  v.arrayValue = new Array();
  v.arrayValue->swap(a);
}

inline surfsara::ast::Node::Value::Value(const Object & o) : typeIndex(std::type_index(typeid(Object)))
{
  v.objectValue = new Object(o);
}

inline surfsara::ast::Node::Value::Value(Object && o) : typeIndex(std::type_index(typeid(Object)))
{
  v.objectValue = new Object();
  v.objectValue->swap(o);
}

inline surfsara::ast::Node::Value::Value(const Value & rhs) : typeIndex(rhs.typeIndex)
{
  init(rhs);
}

inline surfsara::ast::Node::Value::Value(Value && rhs) : typeIndex(rhs.typeIndex)
{
  v = rhs.v;
  rhs.typeIndex = std::type_index(typeid(Null));
}

inline surfsara::ast::Node::Value & surfsara::ast::Node::Value::operator=(const Value & rhs)
{
  cleanup();
  typeIndex = rhs.typeIndex;
  init(rhs);
  return *this;
}

inline surfsara::ast::Node::Value::~Value()
{
  cleanup();
}

template<typename T>
inline bool surfsara::ast::Node::Value::isA() const
{
  return typeIndex == std::type_index(typeid(T));
}

inline void surfsara::ast::Node::Value::init(const Value & rhs)
{
  if(rhs.typeIndex == std::type_index(typeid(String)))
  {
    v.stringValue = new String(*rhs.v.stringValue);
  }
  else if(rhs.typeIndex == std::type_index(typeid(Array)))
  {
    v.arrayValue = new Array(*rhs.v.arrayValue);
  }
  else if(rhs.typeIndex == std::type_index(typeid(Object)))
  {
    v.objectValue = new Object(*rhs.v.objectValue);
  }
  else
  {
    v = rhs.v;
  }
}

inline void surfsara::ast::Node::Value::cleanup()
{
  if(typeIndex == std::type_index(typeid(String)))
  {
    delete v.stringValue;
  }
  else if(typeIndex == std::type_index(typeid(Array)))
  {
    delete v.arrayValue;
  }
  else if(typeIndex == std::type_index(typeid(Object)))
  {
    delete v.objectValue;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Node
//
///////////////////////////////////////////////////////////////////////////////

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
inline surfsara::ast::Node::Node(Undefined a) : value(Undefined()){}
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
  return value.isA<T>();
}

namespace surfsara
{
  namespace ast
  {
    namespace details
    {
      template<typename TARGET>
      struct Converter
      {
      };

      template<>
      struct Converter<Null>
      {
        static const Null & convert(const Node::Value & v)
        {
          return v.v.nullValue;
        }

        static Null & convert(Node::Value & v)
        {
          return v.v.nullValue;
        }
      };

      template<>
      struct Converter<Undefined>
      {
        static const Undefined & convert(const Node::Value & v)
        {
          return v.v.undefinedValue;
        }

        static Undefined & convert(Node::Value & v)
        {
          return v.v.undefinedValue;
        }
      };

      template<>
      struct Converter<Boolean>
      {
        static const Boolean & convert(const Node::Value & v)
        {
          return v.v.booleanValue;
        }

        static Boolean & convert(Node::Value & v)
        {
          return v.v.booleanValue;
        }
      };

      template<>
      struct Converter<Integer>
      {
        static const Integer & convert(const Node::Value & v)
        {
          return v.v.integerValue;
        }

        static Integer & convert(Node::Value & v)
        {
          return v.v.integerValue;
        }
      };

      template<>
      struct Converter<Float>
      {
        static const Float & convert(const Node::Value & v)
        {
          return v.v.floatValue;
        }

        static Float & convert(Node::Value & v)
        {
          return v.v.floatValue;
        }
      };

      template<>
      struct Converter<String>
      {
        static const String  & convert(const Node::Value & v)
        {
          return *v.v.stringValue;
        }

        static String & convert(Node::Value & v)
        {
          return *v.v.stringValue;
        }
      };

      template<>
      struct Converter<Array>
      {
        static const Array  & convert(const Node::Value & v)
        {
          return *v.v.arrayValue;
        }

        static Array  & convert(Node::Value & v)
        {
          return *v.v.arrayValue;
        }

      };

      template<>
      struct Converter<Object>
      {
        static const Object & convert(const Node::Value & v)
        {
          return *v.v.objectValue;
        }

        static Object & convert(Node::Value & v)
        {
          return *v.v.objectValue;
        }
      };
    }
  }
}


template<typename T>
const T& surfsara::ast::Node::as() const
{
  typedef details::Converter<T> converter;
  return converter::convert(value);
}

template<typename T>
T& surfsara::ast::Node::as()
{
  typedef details::Converter<T> converter;
  return converter::convert(value);
  //return boost::get<T>(value);
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
  if(value.typeIndex == rhs.value.typeIndex)
  {
    if(value.typeIndex == std::type_index(typeid(Null)) ||
       value.typeIndex == std::type_index(typeid(Undefined)))
    {
      return true;
    }
    else if(value.typeIndex == std::type_index(typeid(Boolean)))
    {
      return value.v.booleanValue == rhs.value.v.booleanValue;
    }
    else if(value.typeIndex == std::type_index(typeid(Integer)))
    {
      return value.v.integerValue == rhs.value.v.integerValue;
    }
    else if(value.typeIndex == std::type_index(typeid(Float)))
    {
      return value.v.floatValue == rhs.value.v.floatValue;
    }
    else if(value.typeIndex == std::type_index(typeid(String)))
    {
      return *value.v.stringValue == *rhs.value.v.stringValue;
    }
    else if(value.typeIndex == std::type_index(typeid(Array)))
    {
      return *value.v.arrayValue == *rhs.value.v.arrayValue;
    }
    else if(value.typeIndex == std::type_index(typeid(Object)))
    {
      return *value.v.objectValue == *rhs.value.v.objectValue;
    }
  }
  return false;
}

inline bool surfsara::ast::Node::operator!=(const Node & rhs) const
{
  return !(operator==(rhs));
  //return !(value == rhs.value);
}

template<typename Visitor>
void surfsara::ast::Node::applyVisitor(Visitor & visitor) const
{
  if(isA<Null>())
  {
    visitor(as<Null>());
  }
  else if(isA<Undefined>())
  {
    visitor(as<Undefined>());
  }
  else if(isA<Boolean>())
  {
    visitor(as<Boolean>());
  }
  else if(isA<Integer>())
  {
    visitor(as<Integer>());
  }
  else if(isA<Float>())
  {
    visitor(as<Float>());
  }
  else if(isA<String>())
  {
    visitor(as<String>());
  }
  else if(isA<Array>())
  {
    visitor(as<Array>());
  }
  else if(isA<Object>())
  {
    visitor(as<Object>());
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// foreach
//
////////////////////////////////////////////////////////////////////////////////
inline void surfsara::ast::Node::forEach(const std::string & path,
                                         const std::function<void(const Node & root,
                                                                  const std::vector<std::string> & path)> & func) const
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return forEach(vpath, func);
}

inline void surfsara::ast::Node::forEach(const std::vector<std::string> & path,
                                         const std::function<void(const Node & root,
                                                                  const std::vector<std::string> & path)> & func) const
{
  std::vector<std::string> realPath;
  forEachImpl(*this, realPath, path, func, 0);
}

inline void surfsara::ast::Node::forEachImpl(const Node & root,
                                             std::vector<std::string> & realPath,
                                             const std::vector<std::string> & path,
                                             const std::function<void(const Node & root,
                                                                      const std::vector<std::string> & path)> & func,
                                             std::size_t pos) const
{
  if(pos < path.size())
  {
    if(isA<Array>())
    {
      std::string key = path[pos];
      if(key == "*")
      {
        for(std::size_t index = 0; index < as<Array>().size(); index++)
        {
          realPath.push_back(std::to_string(index));
          as<Array>()[index].forEachImpl(root, realPath, path, func, pos + 1);
          realPath.pop_back();
        }
      }
      else
      {
        std::size_t index = getIndexFromString(key, path);
        if(as<Array>().size())
        {
          realPath.push_back(key);
          as<Array>()[index].forEachImpl(root, realPath, path, func, pos + 1);
          realPath.pop_back();
        }
      }
    }
    else if(isA<Object>())
    {
      std::string key = path[pos];
      if(key == "*")
      {
        as<Object>().forEach([&root, &realPath, &func, &path, pos](const std::string & key,
                                                                   const Node & n) 
                    {
                      realPath.push_back(key);
                      n.forEachImpl(root, realPath, path, func, pos + 1);
                      realPath.pop_back();
                    });
      }
      else if(as<Object>().has(key))
      {
        realPath.push_back(key);
        as<Object>()[key].forEachImpl(root, realPath, path, func, pos + 1);
        realPath.pop_back();
      }
    }
  }
  else if(pos == path.size())
  {
    func(root, realPath);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// find
//
////////////////////////////////////////////////////////////////////////////////
inline surfsara::ast::Node surfsara::ast::Node::find(const std::string & path) const
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return find(vpath);
}

inline surfsara::ast::Node surfsara::ast::Node::find(const std::string & path,
                                                     const std::function<bool(const Node & root,
                                                                              const std::vector<std::string> & path)> & pred) const
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return find(vpath, pred);
}

inline surfsara::ast::Node surfsara::ast::Node::find(const std::string & path,
                                                     const std::function<bool(const Node & node)> & pred) const
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return find(vpath,
              [pred](const Node & root, const std::vector<std::string> & path){ return pred(root.find(path));});
}

inline surfsara::ast::Node surfsara::ast::Node::find(const std::vector<std::string> & path) const
{
  std::vector<std::string> realPath;
  return findImpl(*this, realPath, path, [](const Node &, const std::vector<std::string> &){ return true; }, 0);
}

inline surfsara::ast::Node surfsara::ast::Node::find(const std::vector<std::string> & path,
                                                     const std::function<bool(const Node & root,
                                                                              const std::vector<std::string> & path)> & pred) const
{
  std::vector<std::string> realPath;
  return findImpl(*this, realPath, path, pred, 0);
}

inline surfsara::ast::Node surfsara::ast::Node::find(const std::vector<std::string> & path,
                                                     const std::function<bool(const Node & node)> & pred) const
{
  return find(path,
              [pred](const Node & root, const std::vector<std::string> & path){ return pred(root.find(path));});
}


inline surfsara::ast::Node surfsara::ast::Node::findImpl(const Node & root,
                                                         std::vector<std::string> & realPath,
                                                         const std::vector<std::string> & path,
                                                         const std::function<bool(const Node & root,
                                                                                  const std::vector<std::string> & path)> & pred,
                                                         std::size_t pos) const
{
  if(pos < path.size())
  {
    std::string key = path[pos];
    if(isA<Array>())
    {
      if(key == "#")
      {
        return Integer(as<Array>().size());
      }
      else if(key == "*")
      {
        for(std::size_t index = 0; index < as<Array>().size(); index++)
        {
          realPath.push_back(std::to_string(index));
          auto tmp = as<Array>()[index].findImpl(root, realPath, path, pred, pos + 1);
          realPath.pop_back();
          if(tmp != Undefined())
          {
            return tmp;
          }
        }
        return Undefined();
      }       
      else
      {
        std::size_t index = getIndexFromString(key, path);
        if(index < as<Array>().size())
        {
          realPath.push_back(std::to_string(index));
          auto ret = as<Array>()[index].findImpl(root, realPath, path, pred, pos + 1);
          realPath.pop_back();
          return ret;
        }
        else
        {
          return Undefined();
        }
      }
    }
    else if(isA<Object>())
    {
      if(key == "*")
      {
        Node ret = Undefined();
        as<Object>().forEach([&root, &ret, &pred, &realPath, &path, pos](const std::string & key,
                                                                         const Node & node) 
                             {
                               if(ret == Undefined())
                               {
                                 realPath.push_back(key);
                                 ret = node.findImpl(node, realPath, path, pred, pos + 1);
                                 realPath.pop_back();
                               }
                             });
        return ret;
      }
      else if(as<Object>().has(key))
      {
        realPath.push_back(key);
        auto ret = as<Object>()[key].findImpl(root, realPath, path, pred, pos + 1);
        realPath.pop_back();
        return ret;
      }
      else
      {
        return Undefined();
      }
    }
    else
    {
      return Undefined();
    }
  }
  else if(pos == path.size() && pred(root, realPath))
  {
    return *this;
  }
  else
  {
    return Undefined();
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// update
//
////////////////////////////////////////////////////////////////////////////////
inline bool surfsara::ast::Node::update(const std::string & path,
                                        const Node & value,
                                        bool insert)
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return update(vpath, value, insert);
}

inline bool surfsara::ast::Node::update(const std::string & path,
                                        const Node & value,
                                        bool insert,
                                        const Predicate & predicate)
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return update(vpath, value, insert, predicate);
}

inline bool surfsara::ast::Node::update(const std::vector<std::string> & path,
                                        const Node & node,
                                        bool insert)
{
  auto predicate = [](const Node & root, const std::vector<std::string> & path){ return true; };
  UpdateWorkspace ws(*this,  path, node, insert, predicate);
  return updateImpl(ws, 0, false);
}

inline bool surfsara::ast::Node::update(const std::vector<std::string> & path,
                                        const Node & node,
                                        bool insert,
                                        const Predicate & predicate)                                  {
  std::vector<std::string> real_path;
  UpdateWorkspace ws(*this,  path, node, insert, predicate);
  return updateImpl(ws, 0, false);
}


inline bool surfsara::ast::Node::updateImpl(UpdateWorkspace & ws,
                                            std::size_t pos,
                                            bool ignoreUndef)
{
  if(pos < ws.path.size())
  {
    if(isA<Array>())
    {
      return updateArrayImpl(as<Array>(), ws, pos, ignoreUndef);
    }
    else if(isA<Object>())
    {
      return updateObjectImpl(as<Object>(), ws, pos, ignoreUndef);
    }
    else
    {
      if(!ignoreUndef)
      {
        throw PathError(ws.path,
                        std::string("Could update ") + ws.path[pos] +
                        std::string(" in object of type ") + typeName());
      }
      else
      {
        return false;
      }
    }
  }
  else if(pos == ws.path.size())
  {
    if(ws.predicate(ws.root, ws.realPath))
    {
      *this = ws.node;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    throw PathError(ws.path, "Could not decode path");
  }
}

inline bool surfsara::ast::Node::updateArrayImpl(Array & arr,
                                                 UpdateWorkspace & ws,
                                                 std::size_t pos,
                                                 bool ignoreUndef)
{
  std::string key = ws.path[pos];
  if(ws.insert && key == "#")
  {
    ws.realPath.push_back("#");
    bool pred = ws.predicate(ws.root, ws.realPath);
    ws.realPath.pop_back();
    if(pred)
    {
      arr.pushBack(nodeFromPath(ws.path, ws.node, pos + 1));
      return true;
    }
    else
    {
      return false;
    }
  }
  else if(key == "*")
  {
    bool ret = false;
    for(std::size_t index = 0; index < arr.size(); index++)
    {
      ws.realPath.push_back(std::to_string(index));
      ret |= arr[index].updateImpl(ws, pos + 1, true);
      ws.realPath.pop_back();
    }
    return ret;
  }
  else
  {
    std::size_t index = getIndexFromString(key, ws.path);
    if(arr.size())
    {
      ws.realPath.push_back(key);
      arr[index].updateImpl(ws, pos + 1, ignoreUndef);
      ws.realPath.pop_back();
      return true;
    }
    else
    {
      throw PathError(ws.path, std::string("Index out of range ") + key);
    }
  }
}

inline bool surfsara::ast::Node::updateObjectImpl(Object & obj,
                                                  UpdateWorkspace & ws,
                                                  std::size_t pos,
                                                  bool ignoreUndef)
{
  std::string key = ws.path[pos];
  if(key == "*")
  {
    bool ret = false;
    obj.forEach([&ws, &ret, pos](const std::string & key,
                                 Node & n) 
                {
                  ws.realPath.push_back(key);
                  ret |= n.updateImpl(ws, pos + 1, true);
                  ws.realPath.pop_back();
                });
    return ret;
  }
  else if(ws.insert && !obj.has(key) && !ignoreUndef)
  {
    ws.realPath.push_back(key);
    bool pred = ws.predicate(ws.root, ws.realPath);
    ws.realPath.pop_back();
    obj.set(key, nodeFromPath(ws.path, ws.node, pos + 1));
    return pred;
  }
  else if(obj.has(key))
  {
    ws.realPath.push_back(key);
    bool ret = obj[key].updateImpl(ws, pos + 1, ignoreUndef);
    ws.realPath.pop_back();
    return ret;
  }
  else
  {
    return false;
  }
}


////////////////////////////////////////////////////////////////////////////////
//
// remove
//
////////////////////////////////////////////////////////////////////////////////
inline bool surfsara::ast::Node::remove(const std::string & path)
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return remove(vpath);
}

inline bool surfsara::ast::Node::remove(const std::string & path,
                                        const Predicate & predicate)
{
  std::vector<std::string> vpath;
  boost::split(vpath, path, boost::is_any_of("/"));
  return remove(vpath, predicate);
}


inline bool surfsara::ast::Node::remove(const std::vector<std::string> & path)
{
  return remove(path, [](const Node & root,
                         const std::vector<std::string> & path){ return true; });
}

inline bool surfsara::ast::Node::remove(const std::vector<std::string> & path,
                                        const Predicate & predicate)
{
  std::vector<std::string> realPath;
  return removeImpl(*this, realPath, path, predicate, 0, false);
}


inline bool surfsara::ast::Node::removeImpl(const Node & root,
                                            std::vector<std::string> & realPath,
                                            const std::vector<std::string> & path,
                                            const Predicate & predicate,
                                            std::size_t pos,
                                            bool ignoreUndef)
{
  if(pos < path.size())
  {
    if(isA<Array>())
    {
      return removeArrayImpl(as<Array>(), root, realPath, path, predicate, pos, ignoreUndef);
    }
    else if(isA<Object>())
    {
      return removeObjectImpl(as<Object>(), root, realPath, path, predicate, pos, ignoreUndef);
    }
    else if(ignoreUndef)
    {
      return false;
    }
    else
    {
      std::string key = path[pos];
      throw PathError(path,
                      std::string("Could remove ") + key +
                      std::string(" from object of type ") + typeName());
    }
  }
  else if(ignoreUndef)
  {
    return false;
  }
  else
  {
    throw PathError(path, "Could not decoode path");
  }
}

inline bool surfsara::ast::Node::removeArrayImpl(Array & arr,
                                                 const Node & root,
                                                 std::vector<std::string> & realPath,
                                                 const std::vector<std::string> & path,
                                                 const Predicate & predicate,
                                                 std::size_t pos,
                                                 bool ignoreUndef)
{
  std::string key = path[pos];
  if(key == "*")
  {
    bool ret = false;
    if(pos + 1 == path.size())
    {
      for(std::size_t index = 0; index < arr.size();)
      {
        realPath.push_back(std::to_string(index));
        bool pred = predicate(root, realPath);
        if(pred)
        {
          arr.remove(index);
        }
        else
        {
          index++;
        }
        ret|= pred;
        realPath.pop_back();
      }
    }
    else
    {
      for(std::size_t index = 0; index < arr.size(); index++)
      {
        realPath.push_back(std::to_string(index));
        ret |= arr[index].removeImpl(root, realPath, path, predicate, pos + 1, true);
        realPath.pop_back();
      }
    }
    return ret;
  }
  else
  {
    std::size_t index = getIndexFromString(key, path);
    if(index < arr.size())
    {
      if(pos + 1 == path.size())
      {
        realPath.push_back(key);
        bool pred = predicate(root, realPath);
        if(pred)
        {
          arr.remove(index);
        }
        realPath.pop_back();
        return  pred;
      }
      else
      {
        return arr[index].removeImpl(root, realPath, path, predicate, pos + 1, ignoreUndef);
      }
    }
    return false;
  }
}

inline bool surfsara::ast::Node::removeObjectImpl(Object & obj,
                                                  const Node & root,
                                                  std::vector<std::string> & realPath,
                                                  const std::vector<std::string> & path,
                                                  const Predicate & predicate,
                                                  std::size_t pos,
                                                  bool ignoreUndef)
{
  std::string key = path[pos];
  if(key == "*")
  {
    bool ret = false;
    obj.forEach([&root, &realPath, &path, &predicate, &ret, pos](const std::string & key,
                                                                 Node & n) 
                {
                  realPath.push_back(key);
                  ret |= n.removeImpl(root, realPath, path, predicate, pos + 1, true);
                  realPath.pop_back();
                });
    return ret;
  }
  else
  {
    if(obj.has(key))
    {
      if(pos + 1 == path.size())
      {
        realPath.push_back(key);
        bool pred = predicate(root, realPath);
        if(pred)
        {
          obj.remove(path[pos]);
        }
        realPath.pop_back();
        return pred;
      }
      else
      {
        realPath.push_back(key);
        auto ret = obj[path[pos]].removeImpl(root, realPath, path, predicate, pos + 1, ignoreUndef);
        realPath.pop_back();
        return ret;
      }
    }
    return false;
  }
}



////////////////////////////////////////////////////////////////////////////////
//
// auxilary
//
////////////////////////////////////////////////////////////////////////////////
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

inline std::size_t surfsara::ast::Node::getIndexFromString(const std::string & str,
                                                           const std::vector<std::string> & path)
{
  std::stringstream stream(str);
  std::size_t index;
  stream >> index;
  if(stream.fail() || !stream.eof())
  {
    throw PathError(path, std::string("Invalid array index ") + str);
  }
  return index;
}
