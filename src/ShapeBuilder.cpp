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
#include "jet2/ShapeBuilder.hpp"
#include "jet2/Functions.hpp"

namespace jet2 {

ShapeBuilder::ShapeBuilder(Ptr<btCompoundShape> shape, Ptr<sfr::Transform> node) {
// Create a new builder to build a compound shape from the scene-subgraph
// starting at 'node'.  Usually, an OBJ file is loaded in as a node containing
// multiple Mesh objects.  For each Mesh object, generate a btBoxShape using
// the Mesh's bounding box.
    shape_ = shape;
    operator()(node);
}

void ShapeBuilder::operator()(Ptr<sfr::Transform> transform) {
// Calculate the transform matrix given the previous transform + the
// current transform, and store it the result temporarily.
    sfr::Matrix previous = transform_;
    transform_ = transform_ * transform->transform();
    for (sfr::Iterator<sfr::Node> node = transform->children(); node; node++) {
        node(std::static_pointer_cast<ShapeBuilder>(shared_from_this()));
    }
    transform_ = previous;
}

void ShapeBuilder::operator()(Ptr<sfr::Model> node) {
    operator()(node->mesh());
}

void ShapeBuilder::operator()(Ptr<sfr::Mesh> node) {
// Set up the transform for the child shape given the composite of all
// ancestor transforms.  Then, add the child shape to the btCompoundShape
// with this transform.
    auto shape = shapeFor(node);
    auto bounds = node->bounds();
    auto offset = sfr::Matrix::translate((bounds.max+bounds.min)/2.);
    auto transform = transform_ * offset;

    auto pos = transform.origin();
    auto rot = transform.rotation();
    auto btpos = btVector3(pos.x, pos.y, pos.z);
    auto btrot = btQuaternion(rot.x, rot.y, rot.z, rot.w);
    auto bttransform = btTransform(btrot, btpos);
    shape_->addChildShape(bttransform, shape.get());
}

}
