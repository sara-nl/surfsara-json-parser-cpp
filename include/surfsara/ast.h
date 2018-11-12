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
#include <exception>
#include <map>
#include <functional>
#include <type_traits>

namespace surfsara
{
  namespace ast
  {
    class Node;
    
    typedef int64_t Integer;
    typedef bool Boolean;
    typedef std::string String;
    typedef char Char;
    typedef double Float;

    class PathError : public std::exception
    {
    public:
      PathError(const std::vector<std::string> & path,
                const std::string & msg);
      virtual const char* what() const noexcept override
      {
        return msg.c_str();
      }
    private:
      std::string msg;
    };

    struct Null
    {
      inline bool operator==(const Null & rhs) const{ return true; }
    };

    struct Undefined
    {
      inline bool operator==(const Undefined & rhs) const { return true; }
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // Array
    //
    ////////////////////////////////////////////////////////////////////////////
    class Array
    {
    public:
      typedef Node value_type;
      typedef std::vector<value_type>::iterator iterator;

      Array();
      Array(const std::initializer_list<Node> & l);
      inline void forEach(std::function<void(Node & node)> lambda);
      inline void forEach(std::function<void(const Node & node)> lambda)const;
      inline std::size_t size() const;
      inline void pushBack(const Node & node);
      inline void insert(iterator itr, Node value);
      inline void remove(std::size_t i);
      
      inline bool operator==(const Array & rhs) const;
      inline iterator end();
      inline Node & operator[](std::size_t i);
      inline const Node & operator[](std::size_t i) const;
    private:
      std::vector<Node> data;
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // Object
    //
    ////////////////////////////////////////////////////////////////////////////
    class Object
    {
    public:
      typedef std::pair<String, Node> value_type;
      typedef std::map<std::size_t, std::pair<String, Node>>::iterator iterator;
      
      Object();
      Object(const std::initializer_list<std::pair<String, Node>> & l);
      inline bool empty() const;
      inline bool set(const String & k, const Node & node);
      inline bool set(const String & k, Node && node);
      inline bool has(const String & v) const;
      inline Node get(const String & k) const;
      inline Node & operator[](const String & k);
      inline const Node & operator[](const String & k) const;

      /** 
       * calls the lambda function on the node with the key
       * returns true if the key is found false otherwise
       */
      inline bool modify(const String & key, std::function<void(Node & node)> lambda);

      /** 
       * @todo implement
       * calls the lambda function on the node of a nested object with path
       * returns true if the key is found false otherwise
       */
      inline bool modify(const std::vector<String> & path, std::function<void(Node & node)> lambda);

      /**
       * iterate over all elements
       */
      inline void forEach(std::function<void(const String &, Node & node)> lambda);
      inline void forEach(std::function<void(const String &, const Node & node)> lambda) const;

      /**
       * Remove the key if exists
       * return if the keys was found
       */
      inline bool remove(const String &);

      /**
       * Remove all elements that fullfill the predicate
       * returns the numer of removed elements
       */
      inline std::size_t remove(std::function<bool(const String &, const Node & n)> predicate);

      inline bool operator==(const Object & rhs)const{ return this == &rhs; }

      /*@todo use fusion adapter */
      inline iterator end();
      inline void insert(iterator itr, const std::pair<String, Node> & value);

    private:
      template<typename T>
      inline bool setInternal(const String & k, T node);
      std::map<std::size_t, std::pair<String, Node>> data;
      std::unordered_map<String, std::size_t> lookup;
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // Node
    //
    ////////////////////////////////////////////////////////////////////////////
    class Node
    {
    public:
      Node();
      template<typename T>
      Node(T v, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr);
      
      template<typename T>
      Node(T v, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr);
      
      Node(Null a);
      Node(Undefined a);
      Node(Boolean a);
      Node(const Char * str);
      Node(String  a);
      Node(const Array & a);
      Node(const Object & a);
      Node(std::initializer_list<std::pair<String, Node>> a);
      Node(std::initializer_list<Node> a);

      template<typename T>
      bool isA() const;

      template<typename T>
      const T& as() const;

      template<typename T>
      T& as();

      inline bool operator==(const Node & rhs) const;
      inline bool operator!=(const Node & rhs) const;

      inline Node find(const std::string & path) const;
      inline Node find(const std::string & path,
                       const std::function<bool(const Node & root,
                                                const std::vector<std::string> & path)> & pred) const;
      inline Node find(const std::string & path,
                       const std::function<bool(const Node & node)> & pred) const;

      inline Node find(const std::vector<std::string> & path) const;
      inline Node find(const std::vector<std::string> & path,
                       const std::function<bool(const Node & root,
                                                const std::vector<std::string> & path)> & pred) const;
      inline Node find(const std::vector<std::string> & path,
                       const std::function<bool(const Node & node)> & pred) const;

      inline void forEach(const std::string & path,
                          const std::function<void(const Node & root,
                                                   const std::vector<std::string> & path)> & func) const;
      inline void forEach(const std::vector<std::string> & path,
                          const std::function<void(const Node & root,
                                                   const std::vector<std::string> & path)> & func) const;

      inline bool update(const std::string & path,
                         const Node & value,
                         bool insert=true);
      inline bool update(const std::string & path,
                         const Node & value,
                         bool insert,
                         const std::function<bool(const Node & root,
                                                  const std::vector<std::string> & path)> & pred);
    


      inline bool update(const std::vector<std::string> & path,
                         const Node & value,
                         bool insert=true);
      inline bool update(const std::vector<std::string> & path,
                         const Node & value,
                         bool insert,
                         const std::function<bool(const Node & root,
                                                  const std::vector<std::string> & path)> & pred);

      inline bool remove(const std::string & path);
      inline bool remove(const std::string & path,
                         const std::function<bool(const Node & root,
                                                  const std::vector<std::string> & path)> & pred);
      inline bool remove(const std::vector<std::string> & path);
      inline bool remove(const std::vector<std::string> & path,
                         const std::function<bool(const Node & root,
                                                  const std::vector<std::string> & path)> & pred);
      
      template<typename Visitor> 
      typename Visitor::result_type apply_visitor(Visitor & visitor) const; 

      inline std::string typeName() const;

    private:
      typedef std::function<bool(const Node & root,
                                 const std::vector<std::string> & path)> Predicate;
      struct UpdateWorkspace
      {
        const Node & root;
        std::vector<std::string> realPath;
        const std::vector<std::string> & path;
        const Node & node;
        bool insert;
        Predicate predicate;
        UpdateWorkspace(const Node & _root,
                        const std::vector<std::string> & _path,
                        const Node & _node,
                        bool _insert,
                        Predicate _predicate) : root(_root),
                                                path(_path),
                                                node(_node),
                                                insert(_insert),
                                                predicate(_predicate) {}

      };

      inline static std::size_t getIndexFromString(const std::string & str,
                                                   const std::vector<std::string> & path);
      inline Node findImpl(const Node & root,
                           std::vector<std::string> & realPath,
                           const std::vector<std::string> & path,
                           const std::function<bool(const Node & root,
                                                    const std::vector<std::string> & path)> & pred,
                           std::size_t pos) const;
      inline void forEachImpl(const Node & root,
                              std::vector<std::string> & realPath,
                              const std::vector<std::string> & path,
                              const std::function<void(const Node & root,
                                                       const std::vector<std::string> & path)> & func,
                              std::size_t pos) const;

      inline bool removeImpl(const Node & root,
                             std::vector<std::string> & realPath,
                             const std::vector<std::string> & path,
                             const Predicate & predicate,
                             std::size_t pos,
                             bool ignoreUndef);

      inline bool removeArrayImpl(Array & arr,
                                  const Node & root,
                                  std::vector<std::string> & realPath,
                                  const std::vector<std::string> & path,
                                  const Predicate & predicate,
                                  std::size_t pos,
                                  bool ignoreUndef);

      inline bool removeObjectImpl(Object & obj,
                                   const Node & root,
                                   std::vector<std::string> & realPath,
                                   const std::vector<std::string> & path,
                                   const Predicate & predicate,
                                   std::size_t pos,
                                   bool ignoreUndef);

      inline Node nodeFromPath(const std::vector<std::string> & path,
                               const Node & node,
                               std::size_t pos);

      
      inline bool updateImpl(UpdateWorkspace & ws,
                             std::size_t pos,
                             bool ignoreUndef);

      inline bool updateArrayImpl(Array & arr,
                                  UpdateWorkspace & ws,
                                  std::size_t pos,
                                  bool ignoreUndef);
      inline bool updateObjectImpl(Object & obj,
                                   UpdateWorkspace & ws,
                                   std::size_t pos,
                                   bool ignoreUndef);
      
      typedef boost::variant<Integer,
                             Float,
                             Boolean,
                             Null,
                             String,
                             Array,
                             Object,
                             Undefined> Value;
      Value value;
    };

    typedef std::pair<String, Node> Pair;
  } // ns ast
} // ns surfsara

////////////////////////////////////////////////////////////////////////////////
//
// Implementation of Array
//
////////////////////////////////////////////////////////////////////////////////

#include "impl/object.hpp"
#include "impl/array.hpp"
#include "impl/node.hpp"
