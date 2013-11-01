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
#include "Database.hpp"
#include "Shader.hpp"
#include "Context.hpp"
#include "Window.hpp"
#include "DrawBuffer.hpp"
#include "Functions.hpp"
#include "Vector.hpp"

int main() {
    auto db = std::make_shared<Database>();
    //auto context = db->create<Context>("context");
    auto window = db->create<Window>("window");
    auto shader = db->create<Shader>("shader/flat");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glViewport(0, 0, 800, 600);

    auto buf = db->create<DrawBuffer>("buffer/square", DrawBufferType::ATTRIBUTE);
    buf->val(-1.f, -1.f, 0.f);
    buf->val(-1.f, 1.f, 0.f);
    buf->val(1.f, 1.f, 0.f);
    buf->val(1.f, -1.f, 0.f);

    auto draw = ShaderDrawSpec();
    draw.vertices = buf;
    draw.geom = ShaderGeom::QUADS;
    draw.projection = Matrix::perspective(60., 800./600., 1., 100.);
    draw.view = Matrix::identity();
    draw.model = Matrix::translated(Vector(0., 0., -2.));

    while (window->open()) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        sf::Event event;
        while (window->window().pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        shader->draw(draw);
        window->display();
    }
    
    screen_snapshot("foo.png");

    return 0;
}
