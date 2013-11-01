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

std::ostream& operator<<(std::ostream& out, Vector const& vector);
std::istream& operator>>(std::istream& in, Vector& vector);

/* 3-dimensional vector utility class */
class Vector {
public:

    Vector(float x, float y, float z);
    Vector();

    float length() const;
    float lengthSquared() const;
    float distance(Vector const& other) const;
    float distanceSquared(Vector const& other) const;
    float dot(Vector const& other) const;
    Vector cross(Vector const& other) const;
    Vector operator+(Vector const& other) const;
    Vector operator-(Vector const& other) const;
    Vector operator*(Vector const& other) const;
    Vector operator/(Vector const& other) const;
	Vector operator/(float s) const;
    Vector operator-() const;
    Vector operator*(float s) const;
    Vector& operator+=(Vector const& other);
    Vector& operator-=(Vector const& other);
    Vector project(Vector const& other) const;
    Vector unit() const;
    Vector orthogonal() const;
    Vector lerp(Vector const& other, float alpha) const;
    operator const float*() const;
    operator float*();

    friend std::ostream& ::operator<<(std::ostream& out, Vector const& vector);
    friend std::istream& ::operator>>(std::istream& in, Vector& vector);

    bool operator<(Vector const& other) const;
    bool operator==(Vector const& other) const;
    bool operator!=(Vector const& other) const;

    static Vector ZERO;
    static Vector UNIT_X;
    static Vector UNIT_Y;
    static Vector UNIT_Z;

    float x;
    float y;
    float z;
};
