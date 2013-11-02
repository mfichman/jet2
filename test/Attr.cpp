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

#include "jet2/Common.hpp"
#include "jet2/Attr.hpp"
#include <iostream>
#include <cassert>

using namespace jet2;

struct DataStruct {
public:
    DataStruct() : int_const(1), string_const("foo") {}

    AttrLive<int> int_attr;
    AttrLive<std::string> string_attr;

    HashLive<int,int> int_hash;
    HashLive<std::string,std::string> string_hash;

    AttrConst<int> int_const;
    AttrConst<std::string> string_const;

    ArrayLive<int> int_array;
    ArrayLive<std::string> string_array;
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

    ds.int_array.subscribe([&](int32_t key) { assert(key==0); ++events; });
    ds.int_array.push(1);
    ds.int_array.subscribe([&](int32_t key) { assert(key==1); ++events; });
    ds.int_array(1, 2);
    assert(ds.int_array(0)==1);
    assert(ds.int_array(1)==2);

    ds.string_array.subscribe([&](int32_t key) { assert(key==0); ++events; });
    ds.string_array.push("1");
    ds.string_array.subscribe([&](int32_t key) { assert(key==1); ++events; });
    ds.string_array(1, "2");
    assert(ds.string_array(0)=="1");
    assert(ds.string_array(1)=="2");

    assert(ds.int_const()==1);
    assert(ds.string_const()=="foo");

    assert(events==8);

    for (auto elem : ds.string_hash) {
    }

    ds.int_hash.clear();
    ds.int_array.clear();
    assert(ds.int_hash.size()==0);
    assert(ds.int_array.size()==0);

    ArrayConst<int> x = { 1, 2, 3 };

    return 0;
}
