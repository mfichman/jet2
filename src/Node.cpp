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
#include "jet2/Functions.hpp"
#include "jet2/Kernel.hpp"

namespace jet2 {

Node::Node(std::string const& name, Ptr<sfr::Transform> root) : 
    node(scene->root()->childIs<sfr::Transform>(name)),
    mass(massFor(root)),
    shape_(shapeFor(root)) {

    body_.reset(new btRigidBody(mass(), this, shape_.get(), btVector3(0, 1, 0)));
    body_->setUserPointer(this);
    body_->setSleepingThresholds(0.03f, 0.01f);
    world->addRigidBody(body_.get());
    node()->childIs(root);
}

Node::~Node() {
    world->removeCollisionObject(body_.get());
    scene->root()->childDel(node());

}

void Node::getWorldTransform(btTransform& trans) const {
    auto pos = node()->position();
    auto rotation = node()->rotation();
    auto btq = btQuaternion();
    btq.setX(rotation.x);
    btq.setY(rotation.y);
    btq.setZ(rotation.z);
    btq.setW(rotation.w);
    auto btv = btVector3(pos.x, pos.y, pos.z);
    trans = btTransform(btq, btv);
}

void Node::setWorldTransform(btTransform const& trans) {
    auto pos = trans.getOrigin();
    auto rotation = trans.getRotation();
    auto sfrq = sfr::Quaternion(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    auto sfrv = sfr::Vector(pos.x(), pos.y(), pos.z());
    auto matrix = sfr::Matrix(sfrq, sfrv);
    if (matrix != node()->transform()) {
        syncMode = ONCE;
        node()->transformIs(matrix);
    }
}

}
