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

#include "Common.hpp"
#include "Shader.hpp"
#include "DrawBuffer.hpp"
#include "Functions.hpp"
#include "Exception.hpp"

ShaderVertexAttr::ShaderVertexAttr(std::string const& name, GLenum srctype) :
// Defines the format of a vertex as reported by the vertex shader metrics.
    name(name) {

    switch (srctype) {
    case GL_FLOAT: type = GL_FLOAT; components = 1; bytes = sizeof(GLfloat)*components(); break;
    case GL_FLOAT_VEC2: type = GL_FLOAT; components = 2; bytes = sizeof(GLfloat)*components(); break;
    case GL_FLOAT_VEC3: type = GL_FLOAT; components = 3; bytes = sizeof(GLfloat)*components(); break;
    case GL_FLOAT_VEC4: type = GL_FLOAT; components = 4; bytes = sizeof(GLfloat)*components(); break;
    default:
        assert(!"unsupported shader attribute type");
    }
}

Shader::Shader(std::string const& name) : Object(name) {
// Creates and loads a vertex shader.
    load();
}

Shader::~Shader() {
    unload();
}

void Shader::reload() {
    unload();
    load();
}

void Shader::load() {
// Load the shader source from the source file, and determine the shader
// format.  Check for any compilation/link errors.
    if (status() == ShaderStatus::LINKED) { return; }
    
    auto frag_src = read_file(name()+".frag");
    auto frag_len = (GLint)frag_src.size();
    auto frag_lines = frag_src.c_str();
    auto frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &frag_lines, &frag_len);

    auto vert_src = read_file(name()+".vert");
    auto vert_len = (GLint)vert_src.size();
    auto vert_lines = vert_src.c_str();
    auto vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vert_lines, &vert_len);

    id = glCreateProgram();
    glAttachShader(id(), frag_id);
    glAttachShader(id(), vert_id);

    glLinkProgram(id());

    GLint success = 0;
    std::string log;
    glGetProgramiv(id(), GL_LINK_STATUS, &success);
    if (!success) {
        GLint program_len = 0;
        GLint vshader_len = 0;
        GLint fshader_len = 0;
        glGetShaderiv(vert_id, GL_INFO_LOG_LENGTH, &vshader_len);
        glGetShaderiv(frag_id, GL_INFO_LOG_LENGTH, &fshader_len);
        glGetShaderiv(id(), GL_INFO_LOG_LENGTH, &program_len);
        
        log = std::string(program_len+vshader_len+fshader_len+1, '\0');
        auto buf = (GLchar*)log.c_str();
    
        if (vshader_len) {
           glGetShaderInfoLog(vert_id, vshader_len, &vshader_len, buf);
           buf += vshader_len;
        }
        if (fshader_len) {
           glGetShaderInfoLog(frag_id, fshader_len, &fshader_len, buf);
           buf += fshader_len;
        }
        if (program_len) {
           glGetProgramInfoLog(id(), program_len, &program_len, buf);
           buf += program_len;
        }
    }

    glDeleteShader(frag_id);
    glDeleteShader(vert_id);

    if(success) {
        status = ShaderStatus::LINKED;
    } else {
        status = ShaderStatus::ERROR;
        throw ResourceException(log);
    }
    
    view_id = glGetUniformLocation(id(), "view_matrix");
    projection_id = glGetUniformLocation(id(), "projection_matrix");
    model_id = glGetUniformLocation(id(), "model_matrix");

    GLint attrs = 0;
    glGetProgramiv(id(), GL_ACTIVE_ATTRIBUTES, &attrs);
    for (GLuint i = 0; i < attrs; ++i) {
        GLchar name[4096]{}; 
        GLsizei size{};
        GLenum type{};
        glGetActiveAttrib(id(), i, sizeof(name), nullptr, &size, &type, name);
        assert(size == 1 && "unsupported vertex attrib size");
        auto attr = ShaderVertexAttr(name, type);
        vertex_attr.push(attr);
        vertex_size = vertex_size()+attr.bytes();
    }
    // Get the vertex format from the loaded shader info 
}

void Shader::unload() {
// Unload the shader program object.
    if (status() == ShaderStatus::UNLINKED) { return; }

    glDeleteProgram(id());
    id = 0;
}

void Shader::draw(ShaderDrawSpec const& spec) {
// Enable vertex attributes and set up the data format used by the GPU
// according the spec pulled from the shader with glGetActiveAttrib().
    glUniformMatrix4fv(model_id(), 1, 0, spec.model());
    glUniformMatrix4fv(projection_id(), 1, 0, spec.projection());
    glUniformMatrix4fv(view_id(), 1, 0, spec.view());

    glBindBuffer(GL_ARRAY_BUFFER, spec.vertices()->id());
    GLuint index = 0;
    GLuint offset = 0;
    auto const norm = false;
    for(auto va : vertex_attr) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, va.components(), va.type(), norm, vertex_size(), (void*)offset);
        ++index;
        offset += va.bytes();
    }
    GLenum mode = 0;
    switch (spec.geom()) {
    case ShaderGeom::QUADS: mode = GL_QUADS; break;
    case ShaderGeom::TRIANGLES: mode = GL_TRIANGLES; break;
    default: assert(!"invalid ShaderGeom");
    }
    if (spec.indices()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spec.indices()->id());
        GLuint const count = spec.indices()->size()/sizeof(GLuint);
        glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        GLuint const count = spec.vertices()->size()/vertex_size();
        glDrawArrays(mode, 0, count);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



