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
#include "jet2/Database.hpp"
#include "jet2/Window.hpp"
#include "jet2/Kernel.hpp"

namespace jet2 {


Ptr<sfr::WavefrontLoader> meshLoader;
Ptr<sfr::EffectLoader> effectLoader;
Ptr<sfr::TextureLoader> textureLoader;
Ptr<Window> window;

void handleInput() {

}

void init() {
    sf::ContextSettings settings(32, 0, 0, 3, 2);
    sf::VideoMode mode(1200, 800);
    window = db->create<Window>("window", mode, "Window", sf::Style::Default, settings);
    meshLoader = std::make_shared<sfr::WavefrontLoader>(assets);
    effectLoader = std::make_shared<sfr::EffectLoader>(assets);
    textureLoader = std::make_shared<sfr::TextureLoader>(assets);
}

void run() {

    auto shadowRenderer = std::make_shared<sfr::ShadowRenderer>(assets);
    auto deferredRenderer = std::make_shared<sfr::DeferredRenderer>(assets);
    auto updater = std::make_shared<sfr::TransformUpdater>();

    auto clock = sf::Clock(); 
    auto root = scene->root();

    while (window->isOpen()) {
        //auto elapsedTime = clock.getElapsedTime();
        clock.restart();
        
        sf::Event evt;
        while (window->window().pollEvent(evt)) {
            switch (evt.type) {
            case sf::Event::Closed: exit(0); break;
            default: break;
            }
        }

        updater->operator()(scene); 
        shadowRenderer->operator()(scene);
        deferredRenderer->operator()(scene);

        window->display(); 
    }
}

}
