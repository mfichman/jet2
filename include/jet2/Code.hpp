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

#ifdef _WIN32
#define JET2_CODEAPI extern "C" __declspec(dllexport) 
#else
#define JET2_CODEAPI extern "C"
#endif

namespace jet2 {

enum class CodeStatus { LOADED, UNLOADED, ERROR };
using CodeFunc = void (*)(Code*);

class Code : public Object {
// Dynamically loads code from a DLL or shared library for use in the game.
// Each DLL has an entry point (main) which gets called when the module is
// loaded.  Failure to link the main entry point causes the module to be
// disabled.  When the module is unloaded, all actors in the module are
// disabled.
public:
    Code(std::string const& name);
    ~Code() { unload(); }
    void reload();
    void unload();
    void load();

    AttrConst<std::string> name;
    Attr<CodeStatus> status = CodeStatus::UNLOADED;
#ifdef _WIN32
    Attr<HMODULE> handle = nullptr; 
#else
    Attr<void*> handle = nullptr;
#endif

private:
    CodeFunc start = 0;
    CodeFunc stop = 0;
};

}


