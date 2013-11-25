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
#include "jet2/Attr.hpp"

namespace jet2 {

template <typename T>
class Writer {
// Writes indirectly to a buffer for better performance than writing to the raw
// file desctiptor.
public:
    Writer(Ptr<T> fd, size_t size=32768);
    virtual ~Writer() {}

    void write(char* buf, size_t total);
    void flush();
    size_t remaining() { return buffer_.size()-len_; }

private:
    std::vector<char> buffer_;
    Ptr<T> fd_;
    size_t len_;
};

template <typename T>
Writer<T>::Writer(Ptr<T> fd, size_t size) : fd_(fd), len_(0) {
// Allocate space for the buffer
    buffer_.resize(size);
}

template <typename T>
void Writer<T>::write(char* buf, size_t total) {
// Write buf to the internal buffer.  When the buffer is full, flush it to the
// underlying socket/file descriptor
    while (total > 0) {
        if (len_ && total >= buffer_.capacity()) {
            fd_->write(buf, total); 
        } else {
            auto len = std::min(total, remaining());
            auto front = &buffer_.front();
            memcpy(front+len_, buf, len);
            total -= len;
            buf += len;
            len_ += len;
            if (remaining() == 0) {
                flush();
            }
        }
    }
}

template <typename T>
void Writer<T>::flush() {
// Flush to the underlying socket/file descriptor
    fd_->write(&buffer_.front(), buffer_.size());
    len_ = 0;
}

}
