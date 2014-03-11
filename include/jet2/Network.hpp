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
#include "jet2/Object.hpp"
#include "jet2/Connection.hpp"

namespace jet2 {

enum NetworkMode { NONE, CLIENT, SERVER };

typedef uint8_t ClientId;
typedef uint8_t MagicId;
typedef uint8_t NetVersion;

MagicId const MAGIC = 0x24;

class ClientDesc : public Object {
public:
    Attr<MagicId> magic = MAGIC;
    Attr<NetVersion> version = NetVersion(0); 
    Attr<ClientId> clientId = ClientId(0);
    SERIALIZED(magic, version, clientId);
};

class ServerDesc : public Object {
public:
    Attr<MagicId> magic = MAGIC;
    Attr<NetVersion> version = NetVersion(0);
    SERIALIZED(magic, version);
};

class NetworkInfo : public Model {
public:
    // Data regarding the network connection.  This info is used, amongst other
    // things, to detect the RTT for the connection, as used by the client for
    // client-side prediction.
    Attr<uint32_t> pingId;
    SERIALIZED(pingId);
};

}
