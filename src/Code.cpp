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

#include "jet2/Common.hpp" // PCH
#include "jet2/Code.hpp"
#include "jet2/Exception.hpp"

namespace jet2 {

Code::Code(std::string const& name) : Object(name) {
// Create a new code object and load it from the dll/so
    load();
}

void Code::reload() {
// Reload a code object from its source file
    unload();
    load();
}

void Code::load() {
// Load the shared library/dll from the pathname, and invoke the "enter"
// function to set up the dll.  Only loads the dll if it hasn't been loaded
// already.
    if(status() == CodeStatus::LOADED) { return; }
    std::string const path = name()+".dll";
    std::string const tmp = name()+".loaded.dll";
    if (!CopyFile(path.c_str(), tmp.c_str(), false)) { 
        throw ResourceException("couldn't copy library file");
    }
    // Make a copy of the original file, and load that.  This allows the
    // original dll to be recompiled without permission errors, and then
    // reloaded at a later time.
    
    handle = LoadLibrary(tmp.c_str());
    if (!handle()) {
        status = CodeStatus::ERROR;
        throw ResourceException("failed to load library: "+name());
    }
    start = reinterpret_cast<CodeFunc>(GetProcAddress(handle(), "start"));
    if (!start) {
        status = CodeStatus::ERROR;
        throw ResourceException("failed to find entry point for "+name());
    }
    stop = reinterpret_cast<CodeFunc>(GetProcAddress(handle(), "stop"));
    if (!stop) {
        status = CodeStatus::ERROR;
        throw ResourceException("failed to find stop point for "+name());
    }
    start(this);
    status = CodeStatus::LOADED;
}

void Code::unload() {
// Invoke the "exit" function to do any requisite cleanup, and then free the
// shared library/dll.  Only unloads the dll if it isn't already unloaded.
    if (status() == CodeStatus::UNLOADED) { return; }
    stop(this);
    FreeLibrary(handle());
    start = 0;
    stop = 0;
    handle = 0;
    status = CodeStatus::UNLOADED;
}

}