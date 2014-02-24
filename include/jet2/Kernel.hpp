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

namespace jet2 {

class TickListener {
public:
    virtual ~TickListener() {};
    virtual void tick()=0;
};

class RenderListener {
public:
    virtual ~RenderListener() {};
    virtual void render()=0;
};

void init();
void run(); // Run the engine
void exit(); // Quit the program
void tick(); // For coroutines, wait until the next tick event.
void input(); // Wait for next input event

void tickListenerIs(TickListener* listener);
void tickListenerDel(TickListener* listener);
void renderListenerIs(RenderListener* listener);
void renderListenerDel(RenderListener* listener);
// Optimizations over using coroutines to process events (e.g., tick, render,
// etc.).  Coroutine context switching is more expensive than dispatching to a
// handler.  FIXME: Find out a way to make coroutine context switching cheaper
// to allow coroutines to be used extensively.

extern Ptr<Table> const db;
extern Ptr<sfr::AssetTable> const assets;
extern Ptr<sfr::World> const scene;
extern Ptr<btDiscreteDynamicsWorld> world;
extern Ptr<sf::Window> window; // FIXME
extern Ptr<coro::Event> const tickEvent;
extern Ptr<coro::Event> const inputEvent;
extern std::vector<sf::Event> inputQueue; // FIXME
extern coro::Time const timestep;


// Private
void syncTable(Ptr<Table> db);
void recvMessage(Ptr<Table> db, Ptr<Connection> conn);

}
