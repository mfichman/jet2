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

#pragma once

namespace jet2 {

template <typename K, typename V>
class HashConst {
// A constant hash.
public:
    typedef std::unordered_map<K,V> Coll;

    HashConst() {}
    HashConst(typename std::initializer_list<std::pair<K,V>> value) : value_(value) {}
    V const operator()(K const& key) const;
    typename Coll::const_iterator begin() const { return value_.begin(); }
    typename Coll::const_iterator end() const { return value_.end(); }
    size_t size() const { return value_.size(); }

protected:
    Coll value_;
};

template <typename K, typename V>
class Hash : public HashConst<K,V> { 
// A writable hash.
public:
    V const operator()(K const& key) const;
    V const& operator()(K const& key, V const& value) { this->value_[key] = value; return value; }
    void clear() { this->value_.clear(); }
};

template <typename K, typename V>
class HashLive : public Hash<K,V> { 
// A complex attr.  In this case, the attribute is a collection, and it
// contains multiple values indexed by a key.
public:
    typedef std::function<void (K)> Listener;
    typedef std::vector<Listener> ListenerList;

    V const operator()(K const& key) const { return Hash<K,V>::operator()(key); }
    V const& operator()(K const& key, V const& value);
    void clear();
    void subscribe(Listener const& listener) const;

private:
    void notify(K const& key);

    mutable ListenerList listener_;
};

template <typename K, typename V>
V const Hash<K,V>::operator()(K const& key) const {
// Returns the value with key "key" from the collection, or a default V value
// if the element doesn't exist.
    auto i = this->value_.find(key);
    return (i == this->value_.end()) ? V() : i->second;
}

template <typename K, typename V>
V const& HashLive<K,V>::operator()(K const& key, V const& value) {
// Sets the value with key "key" and generates a notification if the value has
// changed.
    if (Hash<K,V>::operator()(key) != value) {
        Hash<K,V>::operator()(key, value);
        notify(key);
    }
    return value;
}

template <typename K, typename V>
void HashLive<K,V>::clear() {
// Clears the entire list, and then generates a notification.
    typename HashConst<K,V>::Coll snapshot;
    snapshot.swap(this->value_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        notify(i->first);
    }
}

template <typename K, typename V>
void HashLive<K,V>::notify(K const& key) {
// Notify all listeners that the value has changed; swap the listener vector
// with an empty one to clear it.
    ListenerList snapshot;
    snapshot.swap(listener_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        (*i)(key);
    } 
}

template <typename K, typename V>
void HashLive<K,V>::subscribe(Listener const& listener) const {
// Subscribe for notifications when the attribute changes.  The "listener"
// function is invoked whenever the attribute is modified.
    listener_.push_back(listener);
}

}
