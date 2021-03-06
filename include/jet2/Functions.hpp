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

#ifdef _WIN32
#define snprintf _snprintf
#endif

namespace jet2 {

std::string readFile(std::string const& path);
void screenSnapshot(std::string const& file);
void slide(Ptr<sfr::Ui> ui, sfr::GLvec2 end, coro::Time duration);
Ptr<btBoxShape> shapeFor(Ptr<sfr::Mesh> mesh);
Ptr<btCompoundShape> shapeFor(Ptr<sfr::Transform> transform);
btScalar massFor(Ptr<sfr::Transform> transform, btScalar density=1.);

template <typename ...Arg>
std::string format(char const* format, Arg... arg) {
    char name[1024];
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
    snprintf(name, sizeof(name), format, arg...);
#ifdef _WIN32
#pragma warning(pop)
#endif
    return std::string(name);
}



}
