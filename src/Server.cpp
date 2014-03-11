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

#include <jet2/Common.hpp>
#include <jet2/Server.hpp>
#include <jet2/Network.hpp>
#include <jet2/Table.hpp>
#include <jet2/Kernel.hpp>

#define log(msg) std::cerr << msg << std::endl;

namespace jet2 {

static void close(WeakPtr<Server> server, ClientId playerId) {
    log("error: connection closed");
    if (auto srv = server.lock()) {
        srv->player(playerId, 0);
        srv->event()->notifyAll();
    }
}

static void send(WeakPtr<Server> server, Ptr<Connection> conn, ClientId playerId, Ptr<Table> db) {
    try {
        send(conn, db);
    } catch (coro::SocketCloseException const&){
        close(server, playerId);
    }
}

static void recv(WeakPtr<Server> server, Ptr<Connection> conn, ClientId playerId, Ptr<Table> db) {
    try {
        recv(conn, db); 
    } catch (coro::SocketCloseException const&){
        close(server, playerId);
    }
}

static void benchmark(Ptr<Connection> conn, Ptr<Table> db) {
// Respond to ping requests & update network info.  FixMe: Prevent from leaking...
    auto in = db->objectIs<NetworkInfo>("input/netinfo");
    auto out = db->objectIs<NetworkInfo>("models/netinfo");

    in->netMode = Model::INPUT;
    out->netMode = Model::OUTPUT;

    for (;;) {
        std::cout << "waiting" << std::endl; 
        in->wait();
        out->pingId = in->pingId();
        out->syncMode = Model::ONCE;
        sendMessage(conn, out);
        conn->writer()->flush();
        std::cout << "ping" << std::endl;
    }
}

static void accept(Ptr<Server> server, Ptr<Connection> conn, Ptr<Table> db) {
// Handle initialization handshake for the client.  Then spawn coroutines to
// sync read/write with the client.  Each sync comes in a "round" indicated the
// update of a round counter for the client.
    auto self = coro::current();
    auto serverDesc = std::make_shared<ServerDesc>();
    auto clientDesc = std::make_shared<ClientDesc>();

    clientDesc->magic = 0;
    try {
        conn->out()->val(serverDesc);
        conn->writer()->flush();
        conn->in()->val(clientDesc);
    } catch (coro::SocketCloseException const&) {
        log("error: connection closed");
        return;
    }
    if (clientDesc->magic() != jet2::MAGIC) {
        log("error: invalid magic number: " << (uint32_t)clientDesc->magic());
        return; // Close connection
    }
    if (clientDesc->clientId() >= server->maxPlayers()) {
        log("error: invalid client id: " << (uint32_t)clientDesc->clientId());
        conn->sd()->close();
        return;
    }
    log("info: client " << (uint32_t)clientDesc->clientId() << " connected");

    auto models = db->objectIs<Table>("models");
    auto input = db->objectIs<Table>("input");
    auto weakServer = WeakPtr<Server>(server);
    auto player = std::make_shared<Player>();
    auto id = clientDesc->clientId();

    player->conn = conn;
    player->id = clientDesc->clientId();
    player->benchmark = coro::start([=]{ benchmark(conn, db); });
    player->send = coro::start([=]{ send(weakServer, conn, id, models); }); 
    player->recv = coro::start([=]{ recv(weakServer, conn, id, input); }); 
    server->player(player->id(), player); 
    server->event()->notifyAll();
    // Create a new player and add it to the server datastructure
}

Ptr<Server> server(Ptr<Table> db, size_t players) {
// Process incoming client connections
    auto port = uint16_t(9090);
    auto ls = std::make_shared<coro::Socket>();
    ls->setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
    ls->bind(coro::SocketAddr("127.0.0.1", port));
    ls->listen(16);  
    log("info: listening on port " << port);

    auto server = std::make_shared<Server>();
    auto weak = WeakPtr<Server>(server);
    server->maxPlayers = players;
    server->accept = coro::start([=]{
        for (;;) {
            auto self = coro::current();
            auto sd = ls->accept();
            sd->setsockopt(IPPROTO_TCP, TCP_NODELAY, true);

            auto srv = weak.lock();
            if (!srv) { return; } // Server died
            log("info: new connection");
            auto conn = std::make_shared<Connection>(sd);
            auto clientc = coro::start([=]{ accept(srv, conn, db); });
            coro::yield(); 
        }
    });
    return server;
}

}
