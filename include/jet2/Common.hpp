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

#ifndef JET2_COMMON_HPP
#define JET2_COMMON_HPP

#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sfr/sfr.hpp>
#include <coro/coro.hpp>
//#include <bullet/btBulletCollisionCommon.h>
//#include <bullet/btBulletDynamicsCommon.h>
#include <sfr/sfr.hpp>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <map>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <initializer_list>

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace jet2 {
class Code;
class Database;
class Exception;
class Object;
}

#endif
