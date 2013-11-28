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
#include "jet2/Kernel.hpp"
#include "jet2/Table.hpp"
#include "jet2/ShapeBuilder.hpp"

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

Ptr<btBoxShape> shapeFor(Ptr<sfr::Mesh> mesh) {
// Return the box shape for a mesh, based on its bounding box.
    auto shape = db->object<btBoxShape>(mesh->name());
    if (!shape) {
        auto bounds = mesh->bounds();
        auto extent = (bounds.max-bounds.min)/2.;
        auto btExtent = btVector3(extent.x, extent.y, extent.z);
        shape = db->objectIs<btBoxShape>(mesh->name(), btExtent);
    }
    return shape;
}

Ptr<btCompoundShape> shapeFor(Ptr<sfr::Transform> node) {
// Recursively build a btCompoundShape made up of the individual bounding boxes
// for each sub-mesh/subtransform of the sfr::Transform.
    auto shape = db->object<btCompoundShape>("shapes/"+node->name());
    if (!shape) {
        shape = db->objectIs<btCompoundShape>("shapes/"+node->name());
        auto sb = std::make_shared<ShapeBuilder>(shape);
        sb->operator()(node);
    }
    return shape;
}

btScalar massFor(Ptr<sfr::Transform> node, btScalar density) {
// Recursively compute the mass of the node using a uniform density and the
// total volume of the mesh bounding boxes nested within the node.
    return 1.;
}

}
