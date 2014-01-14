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
Ptr<sfr::WavefrontLoader> meshLoader;
Ptr<sfr::EffectLoader> effectLoader;
Ptr<sfr::TextureLoader> textureLoader;
Ptr<sfr::TransformUpdater> updater;
Ptr<sfr::BoundsRenderer> boundsRenderer;
Ptr<sfr::ShadowRenderer> shadowRenderer;
Ptr<sfr::DeferredRenderer> deferredRenderer;
Ptr<sfr::SkyboxRenderer> skyboxRenderer;
Ptr<sfr::RibbonRenderer> ribbonRenderer;
Ptr<sfr::BillboardRenderer> billboardRenderer;
Ptr<sfr::ParticleRenderer> particleRenderer;
Ptr<sfr::TransparencyRenderer> transparencyRenderer;

// Physics
Ptr<btDefaultCollisionConfiguration> collisionConfig;
Ptr<btCollisionDispatcher> dispatcher;
Ptr<btDbvtBroadphase> broadphase;
Ptr<btSequentialImpulseConstraintSolver> solver;
Ptr<btDiscreteDynamicsWorld> world;
Ptr<coro::Event> stepEvent;

Ptr<Table> const db = std::make_shared<Table>();
coro::Time const timestep = coro::Time::sec(1./60.);

void tick(btDynamicsWorld* world, btScalar timestep) {
    world->clearForces();
    stepEvent->notifyAll();
    coro::yield();
}

void init() {
    meshLoader.reset(new sfr::WavefrontLoader(assets));
    effectLoader.reset(new sfr::EffectLoader(assets));
    textureLoader.reset(new sfr::TextureLoader(assets));
    updater.reset(new sfr::TransformUpdater);

    collisionConfig.reset(new btDefaultCollisionConfiguration());
    dispatcher.reset(new btCollisionDispatcher(collisionConfig.get()));
    broadphase.reset(new btDbvtBroadphase());
    solver.reset(new btSequentialImpulseConstraintSolver());
    world.reset(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfig.get()));
    stepEvent.reset(new coro::Event);

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
    transparencyRenderer = std::make_shared<sfr::TransparencyRenderer>(assets);

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

Ptr<ModelTable> modelTable(Ptr<Table> db) {
// Create the model/model ID mapping.  The mapping must be identical for both
// sides of the connection.
    auto models = db->object<ModelTable>("mt");
    if (models) { 
        return models; 
    }
    models = db->objectIs<ModelTable>("mt");
    for (auto entry : *db->object<Table>("models")) {
        auto data = entry.second.cast<Model>();
        if (data->id() == 0) {
            models->nextId = models->nextId()+1;
            data->id = models->nextId;
        }
        models->model(data->id(), data);
    }
    return models;
}

void syncConnection(Ptr<Table> db, Ptr<Connection> conn) {
// Synchronize one connection, by sending data for any dirty models.
    for (auto entry : *db->object<Table>("models")) {
        auto model = entry.second.cast<Model>();
        if (model->id() == 0 || model->syncMode() == Model::DISABLED) { 
            continue; 
        }
        conn->out()->val(model->id());
        if (!conn->model(model->id())) {
            uint8_t const flags = jet2::Model::CONSTRUCT;
            conn->out()->val(flags);
            model->construct(conn->out());
            conn->model(model->id(), model);
        } else {
            uint8_t const flags = jet2::Model::SYNC;
            conn->out()->val(flags);
        }
        conn->out()->val(model);
        if (model->syncMode() == Model::ONCE) {
            model->syncMode = Model::DISABLED;
        }
    }
    conn->writer()->flush();
}

void syncTable(Ptr<Table> db) {
// Update the network.  Find all outgoing connections, and broadcast any
// updates to dirty objects.  
    auto models = modelTable(db);
    for (auto entry : *db->object<Table>("connections")) {
        auto conn = entry.second.cast<Connection>();
        if (!conn) { continue; }
        syncConnection(db, conn);
    }
}

void recvMessage(Ptr<Table> db, Ptr<Connection> conn) {
// Receive one message from a connection.
    auto modelId = ModelId(0);
    conn->in()->val(modelId); 

    auto flags = uint8_t(0);
    conn->in()->val(flags); 

    auto models = modelTable(db); 
    auto model = models->model(modelId);
    assert(model && "model not found");
    if (flags == jet2::Model::CONSTRUCT) {
        model->construct(conn->in());
    }
    conn->in()->val(model);
}

void recvConnection(Ptr<Table> db, Ptr<Connection> conn) {
// Receive a stream of messages from a connection
     
}

void sync(sf::Time const& delta) {
    syncTable(db);
}


void input(sf::Time const& delta) {
// Handle window input and dispatch it to any listeners.
    sf::Event evt;
    while (window->pollEvent(evt)) {
        switch (evt.type) {
        case sf::Event::Closed: 
            ::exit(0);
            break;
        case sf::Event::KeyPressed:
            if (evt.key.code == sf::Keyboard::Escape) {
                ::exit(0);
            } else if (evt.key.code == sf::Keyboard::F4 && evt.key.alt) {
                ::exit(0);
            }
            break;
        default: 
            break;
        }
    }
}

void render(sf::Time const& delta) {
// Render one frame of the scene, and display it. 
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    updater->operator()(scene); 
    shadowRenderer->operator()(scene);
    deferredRenderer->operator()(scene);
    skyboxRenderer->operator()(scene);
    billboardRenderer->operator()(scene);
    ribbonRenderer->operator()(scene);
    particleRenderer->operator()(scene);
    transparencyRenderer->operator()(scene);
   // boundsRenderer->operator()(scene);

    window->display(); 
}

void physics(sf::Time const& delta) {
    std::cout << delta.asSeconds() << std::endl;
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
    auto cloop = coro::start(std::bind(task, loop, 120));
    //coro::start(std::bind(task, sync, 60));
    // Run at 60 Hz for better response time
    coro::run();
}

}
