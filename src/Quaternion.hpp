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

std::ostream& operator<<(std::ostream& out, Quaternion const& quaternion);
std::istream& operator>>(std::istream& in, Quaternion& quaternion);

class Quaternion {
public:
    Quaternion(float w, float x, float y, float z);
    Quaternion(Vector const& axis, float angle);
    Quaternion(Vector const& x, Vector const& y, Vector const& z);
    Quaternion(Matrix const& matrix);
	Quaternion(float w, Vector const& vector);
    Quaternion();
    
    friend std::ostream& ::operator<<(std::ostream& out, Quaternion const& quaternion);
    friend std::istream& ::operator>>(std::istream& in, Quaternion& quaternion);

    float length() const;
    float lengthSquared() const;
    float dot(Quaternion const& other) const;
    Vector operator*(Vector const& other) const;
    Quaternion operator*(float s) const;
    Quaternion operator+(Quaternion const& other) const;
    Quaternion operator-(Quaternion const& other) const;
    Quaternion operator*(Quaternion const& other) const;
    Quaternion operator-() const;
    Quaternion slerp(Quaternion const& other, float alpha) const;
    Quaternion inverse() const;
    Quaternion unit() const;
    bool operator==(const Quaternion other) const;
    bool operator!=(Quaternion const& other) const;
	Quaternion& operator+=(Quaternion const& other);
	Quaternion& operator-=(Quaternion const& other);

    static Quaternion IDENTITY;

    float w;
    float x;
    float y;
    float z;
};
