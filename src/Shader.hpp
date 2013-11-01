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
#include "Object.hpp"
#include "Matrix.hpp"

enum class ShaderStatus { UNLINKED, LINKED, ERROR };
enum class ShaderGeom { QUADS, TRIANGLES };

class ShaderVertexAttr {
// Format of a single attribute as reported by glGetActiveAttrib.  Used when
// binding data to the shader during draw() to indicate the format of the data
// that is drawn using glDrawArrays or glDrawElements().
public:
    ShaderVertexAttr(std::string const& name, GLenum srctype);

    AttrConst<std::string> name;
    Attr<GLenum> type;
    Attr<GLint> components;
    Attr<GLint> bytes;
};

class ShaderDrawSpec {
// Data necessary to draw an object.
public: 
    Attr<ShaderGeom> geom;
    Attr<Ptr<DrawBuffer>> vertices;
    Attr<Ptr<DrawBuffer>> indices;
    Attr<Matrix> view;
    Attr<Matrix> projection;
    Attr<Matrix> model;
};

class Shader : public Object {
// Loads a shader from a file, determines the vertex format, and draws vertices
// using an index buffer/vertex buffer onto the screen.
public:
    Shader(std::string const& name);
    ~Shader();
    void reload();
    void load();
    void unload();
    void draw(ShaderDrawSpec const& spec);

    Attr<ShaderStatus> status = ShaderStatus::UNLINKED;
    Attr<GLint> id = 0;
    Array<ShaderVertexAttr> vertex_attr;
    Attr<GLuint> vertex_size = 0; 
    Attr<GLint> view_id = 0;
    Attr<GLint> projection_id = 0;
    Attr<GLint> model_id = 0;

private:
    void check_status();

};

