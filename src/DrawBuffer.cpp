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
#include "DrawBuffer.hpp"

DrawBuffer::DrawBuffer(std::string const& name, DrawBufferType type) : 
    Object(name), 
    type(type) {

    auto tmpid = id();
    glGenBuffers(1, &tmpid);
    id = tmpid;
}

DrawBuffer::~DrawBuffer() {
    auto tmpid = id();
    glDeleteBuffers(1, &tmpid);    
}

void DrawBuffer::load() {
// Loads data into the GPU's memory for processing by a shader.
    if (status() == DrawBufferStatus::SYNCED) { return; }
    GLenum target = 0;
    if (type() == DrawBufferType::ATTRIBUTE) {
        target = GL_ARRAY_BUFFER;
    } else if (type() == DrawBufferType::INDEX) {
        target = GL_ELEMENT_ARRAY_BUFFER;
    } else {
        assert(!"invalid draw buffer type");
    }    
    status = DrawBufferStatus::SYNCED;
    glBindBuffer(target, id());
    glBufferData(target, data.size(), &data.front(), GL_STATIC_DRAW);
    glBindBuffer(target, 0);
}

void DrawBuffer::unload() {
// Mark the buffer as dirty
    if (status() == DrawBufferStatus::DIRTY) { return; }
    status = DrawBufferStatus::DIRTY;
}


