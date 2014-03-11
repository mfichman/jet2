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

#include "jet2/Common.hpp"
#include "jet2/Client.hpp"
#include "jet2/Kernel.hpp"
#include "jet2/Network.hpp"
#include "jet2/Table.hpp"
#define log(msg) std::cerr << msg << std::endl;

using namespace jet2;

static void send(Ptr<Connection> conn, Ptr<Table> db) {
// Send game data continuously, once per timestep.  Also, periodically send a ping in concert with a
    try {
        jet2::send(conn, db);
    } catch (coro::SocketCloseException const&){
        log("error: connection to server closed");
    }
}

static void recv(Ptr<Connection> conn, Ptr<Table> db) {
    try {
        jet2::recv(conn, db);
    } catch (coro::SocketCloseException const&){
        log("error: connection to server closed");
    }
}

static void benchmark(Ptr<Connection> conn, Ptr<Table> db) {
// Collects performance info regarding the connection, for use in client-side
// prediction.  In particular, this coroutine sends a ping, waits for a
// response from the server, and uses that information to estimate the 1/2 RTT
// value.  This value is used to extrapolate the position of objects given
// their velocity, to achieve a better estimate of the position of each object.
    auto in = db->objectIs<NetworkInfo>("remotes/netinfo");
    auto out = db->objectIs<NetworkInfo>("input/netinfo");

    in->netMode = Model::INPUT; 
    out->netMode = Model::OUTPUT;

    auto rttEstimate = 0.0;
    auto mix = 0.5;
    auto timer = sf::Clock();

    // FIXME: Move all this code into jet2, and add events for net send/recv
    for (;;) {
        out->pingId = out->pingId() + 1; 
        out->syncMode = Model::ONCE;
        std::cout << "sending" << std::endl;
        std::cout << out->pingId() << ',' << in->pingId() << std::endl;
        sendMessage(conn, out);
        timer.restart(); 
        conn->writer()->flush(); // Ensure immediate send
        while (in->pingId() < out->pingId()) {
            in->wait();
        }
        std::cout << out->pingId() << ',' << in->pingId() << std::endl;
        
        auto delta = timer.getElapsedTime().asSeconds();
        rttEstimate = mix*rttEstimate + (1-mix)*delta;

        jet2::netDelta = coro::Time::sec(rttEstimate);
        // Divide by 2?
        std::cout << "rtt=" << rttEstimate << std::endl;

        coro::sleep(coro::Time::sec(1));
    }
}

static void connect(Ptr<Client> client, Ptr<Table> db) {
// Connect or reconnect client to the server
    auto sd = std::make_shared<coro::Socket>();
    auto serverDesc = std::make_shared<ServerDesc>();
    auto clientDesc = std::make_shared<ClientDesc>();
    auto conn = std::make_shared<Connection>(sd);

    clientDesc->clientId = client->id;
    serverDesc->magic = 0;

    sd->connect(coro::SocketAddr("127.0.0.1", 9090));  
    sd->setsockopt(IPPROTO_TCP, TCP_NODELAY, true);

    conn->out()->val(clientDesc);
    conn->writer()->flush();
    conn->in()->val(serverDesc);
    assert(serverDesc->magic() == jet2::MAGIC);

    auto remotes = db->objectIs<Table>("remotes");
    auto input = db->objectIs<Table>("input");

    client->conn = conn;
    client->benchmark = coro::start([=] { benchmark(conn, db); } );
    client->send = coro::start([=]{ ::send(conn, input); });
    client->recv = coro::start([=]{ ::recv(conn, remotes); });
}


namespace jet2 {

Ptr<Client> client(Ptr<Table> db, ClientId id) {
// Connect to server
    auto client = std::make_shared<Client>();
    client->id = id;
    connect(client, db);
    return client;
}

}
