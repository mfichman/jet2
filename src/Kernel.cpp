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
#include "jet2/Connection.hpp"
#include "jet2/Model.hpp"

namespace jet2 {


Ptr<sf::Window> window;

// Rendering
Ptr<sfr::World> const scene(new sfr::World);
Ptr<sfr::AssetTable> const assets(new sfr::AssetTable);
Ptr<sfr::WavefrontLoader> const meshLoader(new sfr::WavefrontLoader(assets));
Ptr<sfr::EffectLoader> const effectLoader(new sfr::EffectLoader(assets));
Ptr<sfr::TextureLoader> const textureLoader(new sfr::TextureLoader(assets));
Ptr<sfr::TransformUpdater> const updater(new sfr::TransformUpdater);
Ptr<sfr::BoundsRenderer> boundsRenderer;
Ptr<sfr::ShadowRenderer> shadowRenderer;
Ptr<sfr::DeferredRenderer> deferredRenderer;
Ptr<sfr::SkyboxRenderer> skyboxRenderer;
Ptr<sfr::RibbonRenderer> ribbonRenderer;
Ptr<sfr::BillboardRenderer> billboardRenderer;
Ptr<sfr::ParticleRenderer> particleRenderer;

// Physics
Ptr<btDefaultCollisionConfiguration> const collisionConfig(new btDefaultCollisionConfiguration());
Ptr<btCollisionDispatcher> const dispatcher(new btCollisionDispatcher(collisionConfig.get()));
Ptr<btDbvtBroadphase> const broadphase(new btDbvtBroadphase());
Ptr<btSequentialImpulseConstraintSolver> const solver(new btSequentialImpulseConstraintSolver());
Ptr<btDiscreteDynamicsWorld> const world(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfig.get()));
Ptr<coro::Event> const stepEvent(new coro::Event);

Ptr<Table> const db = std::make_shared<Table>();
coro::Time const timestep = coro::Time::sec(1./60.);

void tick(btDynamicsWorld* world, btScalar timestep) {
    world->clearForces();
    stepEvent->notifyAll();
    coro::yield();
}

void init() {
// Initialize the renderers, asset loaders, database, etc.
    sf::ContextSettings settings(32, 0, 0, 3, 2);
//    sf::VideoMode mode(1920, 1200);
//    window = std::make_shared<sf::Window>(mode, "Window", sf::Style::Fullscreen, settings);
    sf::VideoMode mode(1200, 800);
    window = std::make_shared<sf::Window>(mode, "Window", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);

    settings = window->getSettings();
    if (settings.majorVersion < 3 || (settings.majorVersion == 3 && settings.minorVersion < 2)) {
        throw std::runtime_error("This program requires OpenGL 3.2");
    }

    // OpenGL initialization
#ifdef SFR_USE_GLEW
    glewExperimental = 1;
    auto err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error((char const*)glewGetErrorString(err));
    }
#endif
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    boundsRenderer = std::make_shared<sfr::BoundsRenderer>(assets);
    shadowRenderer = std::make_shared<sfr::ShadowRenderer>(assets);
    deferredRenderer = std::make_shared<sfr::DeferredRenderer>(assets);
    skyboxRenderer = std::make_shared<sfr::SkyboxRenderer>(assets);
    ribbonRenderer = std::make_shared<sfr::RibbonRenderer>(assets);
    billboardRenderer = std::make_shared<sfr::BillboardRenderer>(assets);
    particleRenderer = std::make_shared<sfr::ParticleRenderer>(assets);

    world->setInternalTickCallback(tick, nullptr, true);
}

void task(void (*func)(sf::Time const&), uint64_t hz) {
// Invokes task function 'func' once per the interval given by 'rate'
    auto clock = sf::Clock(); 
    while (true) {
        auto delta = clock.getElapsedTime();
        clock.restart();
        func(delta);
        if (hz) {
            auto used = clock.getElapsedTime();
            auto interval = sf::seconds(1./(double)hz);
            auto sleep = std::max(interval-used, sf::seconds(0));
            coro::sleep(coro::Time::microsec(sleep.asMicroseconds()));
        } else {
            coro::yield();
        }
    }
}


void sync(sf::Time const& delta) {
// Update the network.  Find all outgoing connections, and broadcast any
// updates to dirty objects.  
    for (auto ent : *db->object<Table>("conn")) {
        auto conn = ent.second.cast<Connection>();
        if (!conn) { continue; }
        for (auto obj : *db->object<Table>("data")) {
            auto data = ent.second.cast<Model>();
            if (data->syncMode()==Model::DISABLED) { continue; }
            conn->out()->val(data);
            if (data->syncMode()==Model::ONCE) {
                data->syncMode = Model::DISABLED;
            }
        }
    }
}

void input(sf::Time const& delta) {
// Handle window input and dispatch it to any listeners.
    sf::Event evt;
    while (window->pollEvent(evt)) {
        switch (evt.type) {
        case sf::Event::Closed: ::exit(0); break;
        default: break;
        }
    }
}

void render(sf::Time const& delta) {
// Render one frame of the scene, and display it. 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    updater->operator()(scene); 
    shadowRenderer->operator()(scene);
    deferredRenderer->operator()(scene);
    skyboxRenderer->operator()(scene);
    billboardRenderer->operator()(scene);
    ribbonRenderer->operator()(scene);
    particleRenderer->operator()(scene);
   // boundsRenderer->operator()(scene);

    window->display(); 
}

void physics(sf::Time const& delta) {
    world->stepSimulation(delta.asSeconds(), 8, timestep.sec());
}

void loop(sf::Time const& delta) {
    input(delta);
    physics(delta);
    render(delta);
}

void exit() {
}

void step() {
    // Wait for the physics step event
    stepEvent->wait();
}

void run() {
    auto cloop = coro::start(std::bind(task, loop, 60));
    //coro::start(std::bind(task, sync, 120));
    // Run at 120 Hz for better response time
    coro::run();
}

}
