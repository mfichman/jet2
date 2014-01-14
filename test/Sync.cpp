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
#include "jet2/jet2.hpp"

template <typename T>
using Ptr = jet2::Ptr<T>;

class Ship : public jet2::Model {
public:
    jet2::Attr<std::string> type;
    CONSTRUCT(type);
    SERIALIZED(position);
};

void setup(Ptr<jet2::Table> db) {
    // Set up a sample scene (identical setup on both connection sides)
    auto ship = db->objectIs<Ship>("models/ship1");
    ship->syncMode = jet2::Model::ALWAYS;
}

bool done = false;

void server(Ptr<coro::Event> event) {
    try {
        auto ls = std::make_shared<coro::Socket>();
        ls->setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        ls->bind(coro::SocketAddr("127.0.0.1", 9091));
        ls->listen(10);

        auto sd = ls->accept();
        auto db = std::make_shared<jet2::Table>();
        auto conn = db->objectIs<jet2::Connection>("connections/1", sd);

        setup(db);

        auto ship = db->object<Ship>("models/ship1");
        assert(ship);
        ship->position = sfr::Vector(1, 1, 1);
        ship->type = std::string("foo");

        syncTable(db);

        while (!done) {
            event->wait();
        }

    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
        exit(1);
    }
}

void client(Ptr<coro::Event> event) {
    try {
        auto sd = std::make_shared<coro::Socket>();
        auto db = std::make_shared<jet2::Table>();
        auto conn = db->objectIs<jet2::Connection>("connections/1", sd);
        sd->connect(coro::SocketAddr("127.0.0.1", 9091));

        setup(db);
        recvMessage(db, conn); 

        done = true;
        event->notifyAll();

        auto ship = db->object<Ship>("models/ship1");
        assert(ship->position() == sfr::Vector(1, 1, 1));
        assert(ship->type() == "foo");
        std::cout << "pass" << std::endl;
    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
        exit(1);
    }
}

int main() {
    auto event = std::make_shared<coro::Event>();
    auto cserver = coro::start([&] { server(event); });
    auto cclient = coro::start([&] { client(event); });
    coro::run();
    return 0;
}
