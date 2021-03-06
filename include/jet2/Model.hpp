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

namespace jet2 {

typedef uint16_t ModelId;
class ModelTable : public Object {
public:
    Hash<ModelId, Ptr<Model>> model;
    Attr<ModelId> nextId = ModelId(0);
};

class Model : public Object {
public:
    enum SyncMode { ALWAYS, ONCE, DISABLED };
    enum SyncFlags { CONSTRUCT, SYNC };
    enum NetMode { OUTPUT, INPUT };

    Attr<ModelId> id = ModelId(0);
    Attr<sfr::Vector> position; // FIXME: Move to subclass
    Attr<sfr::Quaternion> rotation;  
    Attr<SyncMode> syncMode = ALWAYS; 
    Attr<NetMode> netMode = OUTPUT;
    Attr<TickId> tickId = 0;

    void wait() { event_.wait(); }
    void notifyAll() { event_.notifyAll(); }

private:
    coro::Event event_; // FIXME: Move to subclass?
};

}
