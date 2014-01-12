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
#include "jet2/Controller.hpp"
#include "jet2/Functions.hpp"
#include "jet2/Kernel.hpp"

namespace jet2 {

Controller::Controller(Ptr<Model> model, Ptr<sfr::Transform> root) {
    mass_ = (root ? massFor(root) : 0);
    model_ = model;
    shape_ = root ? shapeFor(root) : 0;

    if (shape_) {
        btVector3 localInertia;
        shape_->calculateLocalInertia(mass_, localInertia);
        body_.reset(new btRigidBody(mass_, this, shape_.get(), localInertia));
        body_->setUserPointer(this);
        body_->setSleepingThresholds(0.03f, 0.01f);
        body_->setActivationState(DISABLE_DEACTIVATION);
        world->addRigidBody(body_.get());
    }
    coro_ = coro::start([this]() { run(); });
}

Controller::~Controller() {
    if (body_) {
        world->removeCollisionObject(body_.get());
    } 
}

void Controller::run() {
    while (true) {
        tick();
        jet2::step();
    }
}

void Controller::getWorldTransform(btTransform& trans) const {
    auto pos = model_->position();
    auto rotation = model_->rotation();
    auto btq = btQuaternion();
    btq.setX(rotation.x);
    btq.setY(rotation.y);
    btq.setZ(rotation.z);
    btq.setW(rotation.w);
    auto btv = btVector3(pos.x, pos.y, pos.z);
    trans = btTransform(btq, btv);
}

void Controller::setWorldTransform(btTransform const& trans) {
    auto pos = trans.getOrigin();
    auto rotation = trans.getRotation();
    auto sfrv = sfr::Vector(pos.x(), pos.y(), pos.z());
    auto sfrq = sfr::Quaternion(rotation.w(), rotation.x(), rotation.y(), rotation.z());
    model_->position = sfrv; 
    model_->rotation = sfrq;
}

}
