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
#include "jet2/Table.hpp"
#include "jet2/Kernel.hpp"
#include "jet2/Connection.hpp"
#include "jet2/Model.hpp"
#include "jet2/Controller.hpp"

#undef assert
#define assert(x) if (!(x)) { __debugbreak(); }

namespace jet2 {

void assignId(Ptr<ModelTable> mt, Ptr<Model> model, std::string const& name) {
// Assign an ID to the given model, if not already assigned, and add it to the
// model lookup table.
     if (model->id() == 0) {
         mt->nextId = mt->nextId()+1;
         model->id = mt->nextId;
     }
     std::cout << name << " => " << model->id() << std::endl;
     mt->model(model->id(), model);
}

void assignIds(Ptr<ModelTable> mt, Ptr<Table> db) {
// Assign IDs recursively to all models in the database.
    for (auto entry : *db) {
        if (entry.second.cast<Object>() == mt) {
            // Pass
        } else if (auto db = entry.second.cast<Table>()) {
            assignIds(mt, db);
        } else if(auto model = entry.second.cast<Model>()) {
            assignId(mt, model, entry.first);
        } else {
            assert(!"not a model");
        }
    }
}

Ptr<ModelTable> modelTable(Ptr<Table> db) {
// Create the model/model ID mapping.  The mapping must be identical for both
// sides of the connection.
    auto mt = db->object<ModelTable>("mt");
    if (mt) { 
        return mt; 
    }
    mt = db->objectIs<ModelTable>("mt");
    assignIds(mt, db);
    return mt;
}

void sendMessage(Ptr<Connection> conn, Ptr<Model> model) {
// Send a single message for a single model.  Ensure that if another coroutine
// is currently sending a message, the call is blocked until the message is
// atomically sent, to avoid interleaving messages non-atomically.
    if (model->id() == 0 || model->syncMode() == Model::DISABLED || model->netMode() == Model::INPUT) { 
        return;
    }
    while (conn->state() == Connection::SENDING) {
        conn->event.wait();
    }
    conn->state = Connection::SENDING;
    conn->out()->val(model->id());
    if (!conn->model(model->id())) {
        uint8_t const flags = jet2::Model::CONSTRUCT;
        conn->out()->val(flags);
        model->construct(conn->out());
        conn->model(model->id(), model);
    } else {
        uint8_t const flags = jet2::Model::SYNC;
        conn->out()->val(flags);
    }
    conn->out()->val(model);
    if (model->syncMode() == Model::ONCE) {
        model->syncMode = Model::DISABLED;
    }
    conn->state = Connection::IDLE;
    conn->event.notifyAll();
}

void sendMessages(Ptr<Connection> conn, Ptr<Table> db, Ptr<ModelTable> mt) {
// Send a message for each model in the database (recursively)
    for (auto entry : *db) {
        if (entry.second.cast<Object>() == mt) {
        } else if (auto db = entry.second.cast<Table>()) {
            sendMessages(conn, db, mt);
        } else if (auto model = entry.second.cast<Model>()) {
            sendMessage(conn, entry.second.cast<Model>());
        } else {
            assert(!"not a model");
        }
    }
}

void sendFrame(Ptr<Connection> conn, Ptr<Table> db) {
// Send one frame of data
    auto mt = modelTable(db);
    sendMessages(conn, db, mt);
    conn->writer()->flush();
}

void send(Ptr<Connection> conn, Ptr<Table> db) {
// Synchronize one connection, by sending data for any dirty models.
    for (;;) {
        sendFrame(conn, db);
        coro::sleep(netTimestep);
    }
}

void send(Ptr<Connection> conn) {
// Send until the connection dies.  When a frame is complete, wait for the sync
// interval to elapse before sending another frame.
    send(conn, db);
}

void recvMessage(Ptr<Connection> conn, Ptr<ModelTable> mt) {
// Receive one message from a connection.
    auto modelId = ModelId(0);
    conn->in()->val(modelId); 

    auto flags = uint8_t(0);
    conn->in()->val(flags); 

    auto model = mt->model(modelId);
    assert(model && "model not found");
    assert(model->netMode() == Model::INPUT && "received message for non-input model");
    // If is marked INPUT, then the socket shouldn't receive any messages for
    // that model.  Receiving a message indicates a programming error.
    if (flags == jet2::Model::CONSTRUCT) {
        model->construct(conn->in());
    }
    conn->in()->val(model);
    model->tickId = jet2::tickId; // Note the tickId of this model @ message receive
    model->notifyAll();
}

void recvMessage(Ptr<Connection> conn, Ptr<Table> db) {
    auto mt = modelTable(db);
    recvMessage(conn, mt);
}


void recv(Ptr<Connection> conn, Ptr<Table> db) {
// Receive a stream of messages from a connection
    auto mt = modelTable(db);
    for (;;) {
        recvMessage(conn, mt);
    } 
}

void recv(Ptr<Connection> conn) {
// Recieve all messages until connection dies
    recv(conn, db);
}

void sync(sf::Time const& delta) {
    assert(!"not implemented");
}


}
