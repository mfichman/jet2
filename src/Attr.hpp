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

template <typename T>
class Const {
// A constant.  Technically, constants don't require an attr accessor like
// mutable attrs do.  However, for completeness (and to preserve interface
// consistency) it's included here.
public:
    Const(T const& value) : value_(value) {}
    T const& operator()() const { return value_; }

private:
    T value_;
};

template <typename T>
class Attr {
// An attribute that generates events.  An Attr is like a regular C++ class
// attribute on steroids: it supports events, and has hooks to allow coroutines
// to wait for the attr to change.  Each attr has a type "T".  Listeners (which
// are basically functions that take one argument of type "T") are registered
// using the subscribe() function below, and are one-shot -- that is, they must
// be reregistered after each event.

public:
    typedef std::function<void (T)> Listener;
    typedef std::vector<Listener> ListenerList;

    Attr() {} // Creates an empty attr
    Attr(T const& value) : value_(value) {} // Creates an attr w/ an initial val
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
T const& Attr<T>::operator=(T const& value) {
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
void Attr<T>::notify() {
// Notify all listeners that the attr has changed; swap the listener vector
// with an empty one to clear it.
    ListenerList snapshot;
    snapshot.swap(listener_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        (*i)(value_);
    } 
}

template <typename T>
void Attr<T>::subscribe(Listener const& listener) const {
// Subscribe for notifications when the attribute changes.  The "listener"
// function is invoked whenever the attribute is modified.
    listener_.push_back(listener);
}


template <typename K, typename V>
class Hash { 
// A complex attr.  In this case, the attribute is a collection, and it
// contains multiple values indexed by a key.

public:
    typedef std::function<void (K)> Listener;
    typedef std::vector<Listener> ListenerList;

    V const operator()(K const& key) const;
    V const& operator()(K const& key, V const& value);
    typename ListenerList::const_iterator begin() const { return listener_.begin(); }
    typename ListenerList::const_iterator end() const { return listener_.end(); }
    void clear();
    void subscribe(Listener const& listener) const;

private:
    void notify(K const& key);

    std::unordered_map<K,V> value_;
    mutable ListenerList listener_;
};

template <typename K, typename V>
V const Hash<K,V>::operator()(K const& key) const {
// Returns the value with key "key" from the collection, or a default V value
// if the element doesn't exist.
    auto i = value_.find(key);
    return (i == value_.end()) ? V() : i->second;
}

template <typename K, typename V>
V const& Hash<K,V>::operator()(K const& key, V const& value) {
// Sets the value with key "key" and generates a notification if the value has
// changed.
    auto current = value_[key];
    if (current == value) { return value; }
    value_[key] = value;
    notify(key);
    return value;
}

template <typename K, typename V>
void Hash<K,V>::clear() {
// Clears the entire list, and then generates a notification.
    std::unordered_map<K,V> snapshot;
    snapshot.swap(value_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        notify(i->first);
    }
}

template <typename K, typename V>
void Hash<K,V>::notify(K const& key) {
// Notify all listeners that the value has changed; swap the listener vector
// with an empty one to clear it.
    ListenerList snapshot;
    snapshot.swap(listener_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        (*i)(key);
    } 
}

template <typename K, typename V>
void Hash<K,V>::subscribe(Listener const& listener) const {
// Subscribe for notifications when the attribute changes.  The "listener"
// function is invoked whenever the attribute is modified.
    listener_.push_back(listener);
}


