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

class Matrix {
public:
	static Matrix frustum(float l, float r, float b, float t, float n, float f);
	static Matrix ortho(float l, float r, float b, float t, float n, float f);
	static Matrix perspective(float fovy, float aspect, float near, float far);
    static Matrix scale(float sx, float sy, float sz);
    static Matrix rotate(Quaternion const& rotation);
    static Matrix rotate(Vector const& x, Vector const& y, Vector const& z);
    static Matrix look(Vector const& pos, Vector const& at, Vector const& up);
    static Matrix look(Vector const& direction);
    static Matrix translated(Vector const& trans);
    static Matrix identity();

    explicit Matrix(const float data[16]);
    Matrix(float m00, float m01, float m02, float m03,
           float m10, float m11, float m12, float m13,
           float m20, float m21, float m22, float m23,
           float m30, float m31, float m32, float m33);
    Matrix(Quaternion const& rotation, Vector const& trans);
    Matrix();

    Matrix inverse() const;
    Matrix transpose() const;
    Matrix operator*(Matrix const& other) const;
    Vector operator*(Vector const& other) const;
    Frustum operator*(Frustum const& other) const;
    Vector rotate(Vector const& other) const;
    Vector normal(Vector const& other) const;

    operator const float*() const;
    operator float*();
    Vector forward() const;
    Vector right() const;
    Vector up() const;
    Vector origin() const;
    Quaternion rotation() const;
    
    /* Matrix layout (column major, a la OpenGL):
     * 0  4  8  12
     * 1  5  9  13
     * 2  6  10 14
     * 3  7  11 15
    */
    float data[16];
};

