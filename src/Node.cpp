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
#include "jet2/Node.hpp"

namespace jet2 {

void Node::getWorldTransform(btTransform& trans) const {
    auto pos = root()->position();
    auto rotation = root()->rotation();
    auto btq = btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
    auto btv = btVector3(pos.x, pos.y, pos.z);
    trans = btTransform(btq, btv);
}

void Node::setWorldTransform(btTransform const& trans) {
    auto pos = trans.getOrigin();
    auto rotation = trans.getRotation();
    auto sfrq = sfr::Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
    auto sfrv = sfr::Vector(pos.x(), pos.y(), pos.z());
    root()->transformIs(sfr::Matrix(sfrq, sfrv));
}

}
