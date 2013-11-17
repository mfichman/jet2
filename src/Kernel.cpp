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
#include "jet2/Table.hpp"
#include "jet2/Kernel.hpp"

namespace jet2 {


Ptr<sfr::WavefrontLoader> meshLoader;
Ptr<sfr::EffectLoader> effectLoader;
Ptr<sfr::TextureLoader> textureLoader;
Ptr<sf::Window> window;
Ptr<sfr::ShadowRenderer> shadowRenderer;
Ptr<sfr::DeferredRenderer> deferredRenderer;
Ptr<sfr::TransformUpdater> updater;
Ptr<Table> const db = std::make_shared<Table>("db");
Ptr<sfr::World> const scene = std::make_shared<sfr::World>();
Ptr<sfr::AssetTable> const assets = std::make_shared<sfr::AssetTable>();


void init() {
    // Initialize the renderers, asset loaders, database, etc.
    sf::ContextSettings settings(32, 0, 0, 3, 2);
    sf::VideoMode mode(1200, 800);
    window = std::make_shared<sf::Window>(mode, "Window", sf::Style::Default, settings);

    settings = window->getSettings();
    if (settings.majorVersion < 3 || (settings.majorVersion == 3 && settings.minorVersion < 2)) {
        throw std::runtime_error("This program requires OpenGL 3.2");
    }

#ifdef sfr_USE_GLEW
    glewExperimental = 1;
    auto err = glewInit();
    if (GLEW_OK != err) {
        throw ResourceException((char const*)glewGetErrorString(err));
    }
#endif
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    meshLoader = std::make_shared<sfr::WavefrontLoader>(assets);
    effectLoader = std::make_shared<sfr::EffectLoader>(assets);
    textureLoader = std::make_shared<sfr::TextureLoader>(assets);
    shadowRenderer = std::make_shared<sfr::ShadowRenderer>(assets);
    deferredRenderer = std::make_shared<sfr::DeferredRenderer>(assets);
    updater = std::make_shared<sfr::TransformUpdater>();
}

void sync() {
    // Update the network.  Find all outgoing connections, and broadcast any
    // updates to dirty objects.  
    auto clock = sf::Clock(); 
    while (true) {
        clock.restart();
    
        auto elapsedTime = clock.getElapsedTime();
        auto frameTime = sf::seconds(1./100.); // 10 ms
        auto sleepTime = std::max(frameTime - elapsedTime, sf::seconds(0));
        coro::sleep(coro::Time::microsec(sleepTime.asMicroseconds()));
    }
}

void render() {
    // Render one frame of the scene, and display it.  Run physics updates.
    auto clock = sf::Clock(); 
    auto root = scene->root();
    while (true) {
        clock.restart();
    
        sf::Event evt;
        while (window->pollEvent(evt)) {
            switch (evt.type) {
            case sf::Event::Closed: exit(0); break;
            default: break;
            }
        }
    
        updater->operator()(scene); 
        shadowRenderer->operator()(scene);
        deferredRenderer->operator()(scene);
    
        window->display(); 
        auto elapsedTime = clock.getElapsedTime();
        auto frameTime = sf::seconds(1./60.);
        auto sleepTime = std::max(frameTime - elapsedTime, sf::seconds(0));
        coro::sleep(coro::Time::microsec(sleepTime.asMicroseconds()));
    }
}

void run() {
    coro::start(render);
    coro::run();
}

}
