/*
 * Copyright (c) 2013 Matt Fichman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, APEXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "jet2/Common.hpp"
#include "jet2/Object.hpp"
#include "jet2/Attr.hpp"

namespace jet2 {

class TableException {
public:
    TableException(std::string const& message);
    std::string const& message() const { return message_; } 
private:
    std::string message_;
};

class TableEntry {
// Stores type info along with a void ptr, so that the type can be checked when
// an object is removed from the table.  Does not support casting to base
// types; the type given when the TableEntry was constructed must exactly match
// the cast type.
public:
    template <typename T>
    TableEntry(Ptr<T> ptr, typename std::enable_if<!std::is_base_of<Object,T>::value,T>::type* dummy=0) : object_(ptr), type_(typeid(T)) {}
    //TableEntry(Ptr<T> ptr) : object_(ptr), type_(typeid(T)) {}
    TableEntry(Ptr<Object> ptr) : object_(ptr), type_(typeid(Object)) {}
    TableEntry() : type_(typeid(void)) {}

    template <typename T>
    Ptr<T> cast() {
        if (type_ == typeid(Object) && std::is_base_of<Object,T>::value) {
            return std::dynamic_pointer_cast<T>(std::static_pointer_cast<Object>(object_));
        } else {
            return typeid(T) == type_ ? std::static_pointer_cast<T>(object_) : 0;
        }
    }
    
    Ptr<void> ptr() { return object_; }

private:
    void operator=(TableEntry const&) {}
    Ptr<void> object_;
    std::type_info const& type_;
};

class Table : public Object {
// Contains a database of objects for the game, listed by long path name.  In
// addition, the Table can automatically synchronize with a remote Table.
public:
    typedef std::unordered_map<std::string,TableEntry> Coll;

    template <typename T, typename... Arg> 
    Ptr<T> objectIs(std::string const& path, Arg const&...arg) {
        return objectIs<T>(path.c_str(), arg...);
    }

    template <typename T>
    Ptr<T> object(std::string const& path) {
        return object<T>(path.c_str());
    }

    template <typename T, typename... Arg>
    Ptr<T> objectIs(char const* path, Arg const&...arg);

    template <typename T>
    Ptr<T> object(char const* path);

    Coll::iterator begin() { return object_.begin(); }
    Coll::iterator end() { return object_.end(); }

private:
    template <typename T, typename... Arg>
    Ptr<T> leafIs(std::string const& name, Arg const&...arg);

    template <typename T>
    Ptr<T> leafIs(std::string const& name);

    Coll object_;
};

template <typename T, typename... Arg>
Ptr<T> Table::leafIs(std::string const& name, Arg const&...arg) {
    // Instantiate an object with constructor args.  If another object already
    // exists, throw an exception.
    auto entry = object_.find(name);
    if (entry == object_.end()) {
        auto object = std::make_shared<T>(arg...);
        object_.insert(std::make_pair(name, TableEntry(object)));
        return object;
    } else {
        throw TableException("object '"+name+"' already exists");
    }
}

template <typename T>
Ptr<T> Table::leafIs(std::string const& name) {
    // Instantiate an object with no constructor args.  If the object already
    // exists, just return it, as long as the type matches.  Otherwise, throw
    // an exception.
    auto entry = object_.find(name);
    if (entry == object_.end()) {
        auto object = std::make_shared<T>();
        object_.insert(std::make_pair(name, TableEntry(object)));
        return object;
    } else if (Ptr<T> object = entry->second.cast<T>()) {
        return object;        
    } else {
        throw TableException("object '"+name+"' already exists");
    }
}

template <typename T, typename... Arg>
Ptr<T> Table::objectIs(char const* path, Arg const&... arg) {
    // Creates a new object if it doesn't already exist and returns it.  If the
    // object already exists, then this function fails.
    auto ptr = strchr(path, '/');
    assert(*path != '\0'); // String is empty
    assert(ptr != path); // String starts with a '/'

    if (ptr) {
        //  012/   3-0 = 3 len
        auto len = ptr - path;
        auto name = std::string(path, len);
        auto table = leafIs<Table>(name);
        return table->objectIs<T>(ptr+1, arg...);
    } else {
        // Leaf node; create the object here
        auto name = std::string(path);
        return leafIs<T>(name, arg...);
    }
};

template <typename T>
Ptr<T> Table::object(char const* path) {
    // Returns the object at the given path
    auto ptr = strchr(path, '/');
    assert(*path != '\0'); // String is empty
    assert(ptr != path); // String starts with a '/'

    if (ptr) {
        //  012/   3-0 = 3 len
        auto len = ptr - path;
        auto name = std::string(path, len);
        auto ent = object_.find(name);
        if (ent == object_.end()) {
            return 0;
        }
        auto table = ent->second.cast<Table>();
        return table ? table->object<T>(ptr+1) : 0;
    } else {
        // Leaf node; get object here
        auto name = std::string(path);
        auto ent = object_.find(name);
        if (ent == object_.end()) {
            return 0;
        }
        return ent->second.cast<T>();
    }
};


}
