/*
 * Copyright (c) 2014 Matt Fichman
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
#include "jet2/Network.hpp"

namespace jet2 {

class Player : public Object {
public:
    ~Player() { conn()->sd()->close(); }
    AttrConst<Ptr<Connection>> conn;
    Attr<ClientId> id = ClientId(0);
    Attr<Ptr<coro::Coroutine>> recv;
    Attr<Ptr<coro::Coroutine>> send;
    Attr<Ptr<coro::Coroutine>> benchmark;
};

class Server : public jet2::Object {
public:
    Array<Ptr<Player>> player;
    Attr<size_t> maxPlayers;
    Attr<Ptr<coro::Coroutine>> accept;
    Attr<Ptr<coro::Event>> event = new coro::Event;
};

}
