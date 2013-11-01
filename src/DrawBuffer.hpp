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
#include "Object.hpp"

enum class DrawBufferType { INDEX, ATTRIBUTE };
enum class DrawBufferStatus { DIRTY, SYNCED };

class DrawBuffer : public Object {
// Hardware buffer for storing vertex and index data.  The API is intended to
// be similar to the API for drawing vertices using glVertex(), except with the
// ability to use generic attributes.
public:
    DrawBuffer(std::string const& name, DrawBufferType type);
    ~DrawBuffer();

    size_t size() const { return data.size(); }

    template <typename T>
    void val(T arg);

    template <typename T, typename... Arg>
    void val(T arg, Arg... rest);

    void load();
    void unload();

    AttrConst<DrawBufferType> type;
    Attr<DrawBufferStatus> status = DrawBufferStatus::DIRTY;
    Attr<GLuint> id = 0;
private:
    std::vector<char> data;  
};

template <typename T>
void DrawBuffer::val(T arg) {
// Serialize a single value into the hardware buffer for processing by the CPU
    data.reserve(data.size()+sizeof(arg));
    auto ptr = &data.back()+1;
    memcpy(ptr, &arg, sizeof(arg));
    data.resize(data.size()+sizeof(arg));
    unload();
}

template <typename T, typename... Arg>
void DrawBuffer::val(T arg, Arg... rest)  {
// Serialize a bunch of values into the hardware buffer.
    val(arg);
    val(rest...); 
}

