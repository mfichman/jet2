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

class Table : public Object {
// Contains a database of objects for the game, listed by long path name.  In
// addition, the Table can automatically synchronize with a remote Table.
public:
    Table(std::string const& name) : Object(name) {}

    typedef std::unordered_map<std::string,Ptr<Object>> Coll;

    template <typename T, typename... Arg> 
    Ptr<T> objectIs(std::string const& path, Arg...arg) {
        return objectIs<T>(path.c_str(), arg...);
    }

    template <typename T>
    Ptr<T> object(std::string const& path) {
        return object<T>(path.c_str());
    }

    template <typename T, typename... Arg>
    Ptr<T> objectIs(char const* path, Arg...arg);

    template <typename T>
    Ptr<T> object(char const* path);

    Coll::iterator begin() { return object_.begin(); }
    Coll::iterator end() { return object_.end(); }

private:
    Coll object_;
};


template <typename T, typename... Arg>
Ptr<T> Table::objectIs(char const* path, Arg... arg) {
    // Creates a new object if it doesn't already exist and returns it 
    auto ptr = strchr(path, '/');
    assert(*path != '\0'); // String is empty
    assert(ptr != path); // String starts with a '/'

    if (ptr) {
        //  012/   3-0 = 3 len
        auto len = ptr - path;
        auto name = std::string(path, len);
        auto obj = object_[name];
        auto table = std::dynamic_pointer_cast<Table>(obj);
        assert(obj==table); // Object already exists and it isn't a table
        if (!table) {
            table = std::make_shared<Table>(name);
            object_[name] = table;
        }
        return table->objectIs<T>(ptr+1, arg...);
    } else {
        // Leaf node; create the object here
        auto name = std::string(path);
        auto ret = object_[name];
        assert(!ret); // Object already exists
        ret = std::make_shared<T>(name, arg...);
        object_[name] = ret;
        return std::dynamic_pointer_cast<T>(ret);
    }
};

template <typename T>
Ptr<T> Table::object(char const* path) {
    // Creates a new object if it doesn't already exist and returns it 
    auto ptr = strchr(path, '/');
    assert(*path != '\0'); // String is empty
    assert(ptr != path); // String starts with a '/'

    if (ptr) {
        //  012/   3-0 = 3 len
        auto len = ptr - path;
        auto name = std::string(path, len);
        auto obj = object_[name];
        auto table = std::dynamic_pointer_cast<Table>(obj);
        return table ? table->object<T>(ptr+1) : 0;
    } else {
        // Leaf node; create the object here
        auto name = std::string(path);
        auto ret = object_[name];
        return std::dynamic_pointer_cast<T>(ret);
    }
};


}
