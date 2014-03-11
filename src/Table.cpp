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
#include "jet2/Table.hpp"

namespace jet2 {

TableException::TableException(std::string const& message) {
    message_ = message;
    std::cerr << "error: " << message << std::endl;
#pragma warning(push)
#pragma warning(disable: 4996)
    if (!getenv("JET2_NO_DEBUG_EXCEPTIONS")) {
#pragma warning(pop)
#ifdef _WIN32
        __debugbreak();
#else
        abort();
#endif
    }
    // By default, exceptions will abort the process, as per SFR's "panic"
    // -based exception model (exceptions indicate a programming error or
    // unrecoverable missing resource).   However, this behavior can be changed
    // via the above environment variable.
}
}


