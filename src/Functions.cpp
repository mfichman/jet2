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
#include "jet2/Functions.hpp"
#include "jet2/Exception.hpp"

namespace jet2 {

std::string readFile(std::string const& path) {
// Read the whole file into a single string.  If the file can't be read, throw
// an exception.
    std::string ret;
    std::ifstream fd(path.c_str());
    if (fd.fail()) {
        throw ResourceException("couldn't read "+path);
    }
    fd.seekg(0, std::ios::end);
    ret.resize((size_t)fd.tellg()+1);
    fd.seekg(0, std::ios::beg);
    fd.read(&ret.front(), ret.size());
    return ret;
}

void screenSnapshot(std::string const& file) {
// Write the context to a file
    GLint ret[4];
    glGetIntegerv(GL_VIEWPORT, ret);
    GLuint const width = ret[2];
    GLuint const height = ret[3];
    std::vector<GLuint> data(width*height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &data.front());

    sf::Image image;
    image.create(width, height, (uint8_t const*)&data.front());
    image.saveToFile(file);
}


btVector3 boundingBox(Ptr<sfr::Transform> node) {

    return btVector3();
}
}
