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

#pragma once

#include "jet2/Common.hpp"
#include "jet2/Object.hpp"
#include "jet2/Functor.hpp"
#include "jet2/Model.hpp"

namespace jet2 {

class Controller : private virtual btMotionState, public Object {
public:
    Controller(Ptr<Model> model, Ptr<sfr::Transform> root);
    virtual ~Controller();
    void getWorldTransform(btTransform& trans) const;
    void setWorldTransform(btTransform const& trans);

    Ptr<btRigidBody> body() const { return body_; }
    Ptr<btCompoundShape> shape() const { return shape_; }
    Ptr<Model> model() const { return model_; }
    btScalar mass() const { return mass_; }

private:
    btScalar mass_;
    Ptr<Model> model_;
    Ptr<btCompoundShape> shape_;
    Ptr<btRigidBody> body_;
};

}
