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

template <typename T>
class ArrayConst {
// A constant array.  The collection is indexed by an int32_t key.
public:
    typedef std::vector<T> Coll;

    ArrayConst() {}
    ArrayConst(std::initializer_list<T> value) : value_(value) {}
    T const operator()(int32_t key) const;
    size_t size() const { return value_.size(); }
    typename ArrayConst<T>::Coll::const_iterator begin() const { return value_.begin(); }
    typename ArrayConst<T>::Coll::const_iterator end() const { return value_.end(); }

protected:
    Coll value_;
};

template <typename T>
class Array : public ArrayConst<T> {
// A writable array.  The collection is indexed by an int32_t key.
public:
    Array() {} 
    T const operator()(int32_t key) const { return ArrayConst<T>::operator()(key); }
    T const& operator()(int32_t key, T const& value);
    void push(T const& value) { this->value_.push_back(value); }
    void clear() { this->value_.clear(); }
};

template <typename T>
class ArrayLive : public Array<T> {
public:
    typedef std::function<void (int32_t)> Listener;
    typedef std::vector<Listener> ListenerList;
    
    T const operator()(int32_t key) const { return Array<T>::operator()(key); }
    T const& operator()(int32_t key, T const& value);
    void push(T const& value);
    void clear();
    void subscribe(Listener const& listener) const;

private:
    void notify(int32_t key);
    
    mutable ListenerList listener_;
};


template <typename T>
T const ArrayConst<T>::operator()(int32_t key) const { 
// Returns the nth element in the collection, or the default T otherwise.  A
// negative number returns the nth element from the end of the collection.
    int32_t const index = key < 0 ? this->value_.size() + key : key;
    if (index < 0) {
        return T();
    } else if (size_t(index) >= this->value_.size()) {
        return T(); 
    } else {
        return this->value_[index];
    }
}

template <typename T>
T const& Array<T>::operator()(int32_t key, T const& value) {
// Replaces the nth element in the collection.  A negative number replaces the
// nth element from the end.
    int32_t const index = key < 0 ? this->value_.size() + key : key;
    if (index < 0) {
        assert(!"index is out-of-range");
    } else {
        if (size_t(index) >= this->value_.size()) {
            this->value_.resize(index+1);
        }
        this->value_[index] = value;
    }
    return value;
}

template <typename T>
T const& ArrayLive<T>::operator()(int32_t key, T const& value) {
// Replaces the nth element in the collection.  A negative number replaces the
// nth element from the end.
    if (Array<T>::operator()(key) != value) {
        Array<T>::operator()(key, value);
        notify(key);
    }
    return value;
} 

template <typename T>
void ArrayLive<T>::push(T const& value) {
// Adds an element to the end of the array.
    Array<T>::push(value);
    notify(this->value_.size()-1);
}

template <typename T>
void ArrayLive<T>::clear() {
// Clears the array, and notifies all listeners
    typename ArrayConst<T>::Coll snapshot;
    snapshot.swap(this->value_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        notify(*i);
    }
}

template <typename T>
void ArrayLive<T>::subscribe(Listener const& listener) const {
// Subscribe for notifications when the attribute changes.  The "listener"
// function is invoked whenever the attribute is modified.
    listener_.push_back(listener);
}

template <typename T>
void ArrayLive<T>::notify(int32_t key) {
// Notify all listeners that the value has changed; swap the listener vector
// with an empty one to clear it.
    ListenerList snapshot;
    snapshot.swap(listener_);
    for (auto i = snapshot.begin(); i != snapshot.end(); ++i) {
        (*i)(key);
    } 
}

}
