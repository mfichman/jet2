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

class Database : public std::enable_shared_from_this<Database> {
// Contains a database of objects for the game, listed by long path name.  In
// addition, the Database can automatically synchronize with a remote Database.
public:
    template <typename T, typename... Arg> 
    Ptr<T> create(std::string const& path, Arg...);

    Hash<std::string,Ptr<Object>> object;
};

template <typename T, typename... Arg>
Ptr<T> Database::create(std::string const& path, Arg... arg) {
    // Creates a new object if it doesn't already exist and returns it 
    auto ret = object(path);
    if (!ret) {
        ret = object(path, std::make_shared<T>(path, arg...));
    }
    return std::static_pointer_cast<T>(ret);
};

}
