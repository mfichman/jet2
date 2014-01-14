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
#include "jet2/Attr.hpp"

namespace jet2 {

#define SERIALIZED(...) \
    void visit(Ptr<jet2::Functor> out) { \
        out->vals(__VA_ARGS__);\
    }

#define CONSTRUCT(...) \
    void construct(Ptr<jet2::Functor> out) { \
        out->vals(__VA_ARGS__);\
    }

class Functor : public std::enable_shared_from_this<Functor> {
public:
    virtual ~Functor() {}

    template <typename V>
    typename std::enable_if<std::is_scalar<V>::value>::type
    val(V& in) {
        val((char*)&in, sizeof(in));
    }

    template <typename V>
    typename std::enable_if<!std::is_scalar<V>::value>::type
    val(Ptr<V> in) {
        in->visit(shared_from_this());
    }

    template <typename V>
    void
    val(Attr<V>& in) {
        val(in.ref());
    }

    void
    val(std::string& in) {
        auto len = in.size();
        val(len); 
        in.resize(len);
        val((char*)in.c_str(), len);
        in.resize(len);
    }

    template <typename V>
    typename std::enable_if<!std::is_scalar<V>::value>::type
    val(V& in) {
        visit(shared_from_this(), in);
    }

    template <typename V, typename ...Arg>
    void
    vals(V& head, Arg&...arg) {
        val(head);
        vals(arg...);
    }
    
    void vals() {}

protected:
    virtual void val(char* buf, size_t len)=0;
};



template <typename T>
class WriteFunctor : public virtual Functor {
public:
    WriteFunctor(Ptr<T> fd) : fd_(fd) {}

private:
    virtual void val(char* buf, size_t len) {
        fd_->write(buf, len);
    }
    Ptr<T> fd_;
};

template <typename T>
class ReadFunctor : public virtual Functor {
public:
    ReadFunctor(Ptr<T> fd) : fd_(fd) {}

private:
    virtual void val(char* buf, size_t len) {
        fd_->read(buf, len);
    }
    Ptr<T> fd_;
};

inline void visit(Ptr<Functor> fn, sfr::Vector& val) {
    fn->vals(val.x, val.y, val.z);
}

inline void visit(Ptr<Functor> fn, sfr::Quaternion& val) {
    fn->vals(val.x, val.y, val.z, val.w);
}


}


