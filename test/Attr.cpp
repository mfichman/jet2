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

#include "Common.hpp"
#include "Attr.hpp"
#include <iostream>
#include <cassert>

struct DataStruct {
public:
    DataStruct() : int_const(1), string_const("foo") {}

    Attr<int> int_attr;
    Attr<std::string> string_attr;

    Hash<int,int> int_hash;
    Hash<std::string,std::string> string_hash;

    Const<int> int_const;
    Const<std::string> string_const;
};

int main() {
    int events = 0;
    auto ds = DataStruct();
    
    ds.int_attr.subscribe([&](int val) { assert(val==1); ++events; });
    ds.int_attr = 1;
    assert(ds.int_attr()==1);

    ds.string_attr.subscribe([&](std::string val) { assert(val=="foo"); ++events; });
    ds.string_attr = "foo";
    assert(ds.string_attr()=="foo");

    ds.int_hash.subscribe([&](int key) { assert(key==2); ++events; });
    ds.int_hash(2,3);
    assert(ds.int_hash(2)==3);

    ds.string_hash.subscribe([&](std::string key) { assert(key=="foo"); ++events; });
    ds.string_hash("foo", "bar");
    assert(ds.string_hash("foo")=="bar");

    assert(ds.int_const()==1);
    assert(ds.string_const()=="foo");

    assert(events==4);

    for (auto elem : ds.string_hash) {
    }

    return 0;
}
