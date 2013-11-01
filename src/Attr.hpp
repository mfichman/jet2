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

#include "Common.hpp"
#include "Hash.hpp"
#include "Array.hpp"

template <typename T>
class AttrConst {
// A constant.  Technically, constants don't require an attr accessor like
// mutable attrs do.  However, for completeness (and to preserve interface
// consistency) it's included here.
public:
    template <typename... Arg>
    AttrConst(Arg... arg) : value_(arg...) {}

    AttrConst(T const& value) : value_(value) {}
    T const& operator()() const { return value_; }

protected:
    T value_;
};

template <typename T>
class Attr {
// A writable attribute.  Provides a setter along with the getter.
public:
    template <typename... Arg>
    Attr(Arg... arg) : value_(arg...) {}

    Attr() {}
    Attr(T const& value) : value_(value) {} // Creates an attr w/ an initial val
    T const& operator=(T const& value) { value_ = value; return value; } 
    T const& operator()(T const& value) { return *this = value; }
    T const& operator()() const { return value_; }
    T& operator()() { return value_; }

    T value_;
};

template <typename T>
class AttrLive {
// An attribute that generates events.  An Attr is like a regular C++ class
// attribute on steroids: it supports events, and has hooks to allow coroutines
// to wait for the attr to change.  Each attr has a type "T".  Listeners (which
// are basically functions that take one argument of type "T") are registered
// using the subscribe() function below, and are one-shot -- that is, they must
// be reregistered after each event.
public:
    typedef std::function<void (T)> Listener;
    typedef std::vector<Listener> ListenerList;

    AttrLive() {} // Creates an empty attr
    AttrLive(T const& value) : value_(value) {} // Creates an attr w/ an initial val
    T const& operator=(T const& value);
    T const& operator()(T const& value) { return *this = value; }
    T const& operator()() const { return value_; }
    void subscribe(Listener const& listener) const;

private:
    void notify();

    T value_;
    mutable ListenerList listener_;
};

template <typename T>
T const& AttrLive<T>::operator=(T const& value) {
// Assign a value to the attribute.  When used inside a composite class, this
// operator provides rather nice syntax for mutation: obj.attr = 10, for
// example.  If the value set for an attr changes, then an event is created an
// all registered listeners are notified.
    if (value == value_) { return value_; }
    value_ = value;
    notify();
    return value_;
}

template <typename T>
void AttrLive<T>::notify() {
// Notify all listeners that the attr has changed; swap the listener vector
// with an empty one to clear it.
    ListenerList snapshot;
    snapshot.swap(listener_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        (*i)(value_);
    } 
}

template <typename T>
void AttrLive<T>::subscribe(Listener const& listener) const {
// Subscribe for notifications when the attribute changes.  The "listener"
// function is invoked whenever the attribute is modified.
    listener_.push_back(listener);
}


