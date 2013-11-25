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

namespace jet2 {

void Code::load() {
// Load the shared library/so from the pathname, and invoke the "enter"
// function to set up the so.  Only loads the so if it hasn't been loaded
// already.
    if(status() == CodeStatus::LOADED) { return; }
#ifdef __APPLE__
    std::string const path = name()+".dylib";
#else
    std::string const path = name()+".so";
#endif
    
    handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle()) {
        status = CodeStatus::ERROR;
        throw ResourceException("failed to load library: "+name());
    }
    start = reinterpret_cast<CodeFunc>(dlsym(handle(), "start"));
    if (!start) {
        status = CodeStatus::ERROR;
        throw ResourceException("failed to find entry point for "+name());
    }
    stop = reinterpret_cast<CodeFunc>(dlsym(handle(), "stop"));
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
    dlclose(handle());
    start = 0;
    stop = 0;
    handle = 0;
    status = CodeStatus::UNLOADED;
}

}
