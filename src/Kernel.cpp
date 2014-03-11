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
#include "jet2/Model.hpp"
#include "jet2/Controller.hpp"

namespace jet2 {


std::vector<TickListener*> tickListener;
std::vector<RenderListener*> renderListener;
std::vector<sf::Event> inputQueue;

Ptr<sf::Window> window;

// Rendering
Ptr<sfr::World> const scene(new sfr::World);
Ptr<sfr::AssetTable> const assets(new sfr::AssetTable);
Ptr<sfr::Interface> assetLoader;
Ptr<sfr::TransformUpdater> updater;
Ptr<sfr::BoundsRenderer> boundsRenderer;
Ptr<sfr::DeferredRenderer> deferredRenderer;

// Physics
Ptr<btDefaultCollisionConfiguration> collisionConfig;
Ptr<btCollisionDispatcher> dispatcher;
Ptr<btDbvtBroadphase> broadphase;
Ptr<btSequentialImpulseConstraintSolver> solver;
Ptr<btDiscreteDynamicsWorld> world;
Ptr<coro::Event> const tickEvent(new coro::Event);
Ptr<coro::Event> const inputEvent(new coro::Event);
Ptr<coro::Event> const renderEvent(new coro::Event);

Ptr<Table> const db = std::make_shared<Table>();
coro::Time const timestep = coro::Time::sec(1./60.);
coro::Time const netTimestep = coro::Time::millisec(100);
coro::Time netDelta;
TickId tickId = 0;
TickId netTickId = 0;

void tick(btDynamicsWorld* world, btScalar timestep) {
// Run a single collision tick.  Clear forces, notify controllers of any
// collisions, and then notify any coroutines that are waiting on the tick
// callback event.
    tickId++;
    world->clearForces();

    auto dispatcher = world->getDispatcher();
    for (auto i = 0; i < dispatcher->getNumManifolds(); ++i) {
        auto manifold = dispatcher->getManifoldByIndexInternal(i);
        auto a = static_cast<btCollisionObject const*>(manifold->getBody0());
        auto b = static_cast<btCollisionObject const*>(manifold->getBody1());
        if (manifold->getNumContacts() > 0) {
            auto ca = std::static_pointer_cast<Controller>(static_cast<Controller*>(a->getUserPointer())->shared_from_this());
            auto cb = std::static_pointer_cast<Controller>(static_cast<Controller*>(b->getUserPointer())->shared_from_this());
            auto pa = manifold->getContactPoint(0).getPositionWorldOnA();
            auto pb = manifold->getContactPoint(0).getPositionWorldOnB();
            ca->collision(cb, pa);
            cb->collision(ca, pb);
        }
    }

    for (auto listener : tickListener) {
        listener->tick();
    }

    tickEvent->notifyAll();
    coro::yield();
}

void initWindow() {
    // Initialize the renderers, asset loaders, etc.
    assetLoader.reset(new sfr::AssetLoader(assets));
    updater.reset(new sfr::TransformUpdater);

    sf::ContextSettings settings(32, 0, 0, 3, 2);
    //sf::VideoMode mode(1920, 1200);
    //sf::VideoMode mode(1200, 800);
    //window = std::make_shared<sf::Window>(mode, "Window", sf::Style::Fullscreen, settings);
    //sf::VideoMode mode(1600, 1000);
    //sf::VideoMode mode(800, 600);
    sf::VideoMode mode(1200, 800);
    window = std::make_shared<sf::Window>(mode, "Window", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
    //window->setMouseCursorVisible(false);

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
    deferredRenderer = std::make_shared<sfr::DeferredRenderer>(assets);
}

void init(KernelMode mode) {
    // Initialize phyics, networking, GUI (if enabled)
    if (world) {
        return;
    }
    collisionConfig.reset(new btDefaultCollisionConfiguration());
    dispatcher.reset(new btCollisionDispatcher(collisionConfig.get()));
    broadphase.reset(new btDbvtBroadphase());
    solver.reset(new btSequentialImpulseConstraintSolver());
    world.reset(new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfig.get()));
    world->setInternalTickCallback(tick, nullptr, true);

    if (mode == NORMAL) {
        initWindow();
    } else if (mode == HEADLESS) {
        assetLoader.reset(new sfr::WavefrontLoader(assets));
    } else {
        assert(!"invalid kernel mode");
    }
}

void task(void (*func)(sf::Time const&), uint64_t hz) {
// Invokes task function 'func' once per the interval given by 'rate'
    auto clock = sf::Clock(); 
    for (;;) {
        auto delta = clock.getElapsedTime();
        clock.restart();
        func(delta);
        if (hz) {
            auto used = clock.getElapsedTime();
            auto interval = sf::seconds(1.f/(float)hz);
            auto sleep = std::max(interval-used, sf::seconds(0));
            coro::sleep(coro::Time::microsec(sleep.asMicroseconds()));
        } else {
            coro::yield();
        }
    }
}

void input(sf::Event const& evt) {
// Handle a single input event
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
        inputQueue.push_back(evt);
        break;
    case sf::Event::TextEntered: // Fallthrough
    case sf::Event::JoystickButtonPressed: // Fallthrough
    case sf::Event::MouseButtonPressed: // Fallthrough
    case sf::Event::MouseWheelMoved: // Fallthrough
    case sf::Event::MouseMoved: // Fallthrough
        inputQueue.push_back(evt);
        break;
    default: 
        break;
    }
}

void input(sf::Time const& delta) {
// Handle window input and dispatch it to any tickListener.
    if (!deferredRenderer) {
        return; // Input disabled;
    }

    sf::Event evt;
    inputQueue.clear();
    while (window->pollEvent(evt)) {
        input(evt);
    }
    if (!inputQueue.empty()) {
        inputEvent->notifyAll();
    }
}

void render(sf::Time const& delta) {
// Render one frame of the scene, and display it. 
    if (!deferredRenderer) {
        return; // Rendering disabled
    }

    renderEvent->notifyAll();
    coro::yield();
    for (auto listener : renderListener) {
        listener->render();
    }

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    deferredRenderer->operator()(scene);
   // boundsRenderer->operator()(scene);
}

void physics(sf::Time const& delta) {
/*
    if (delta.asSeconds() > .018) {
        std::cout << delta.asSeconds() << std::endl;
    }
*/
    world->stepSimulation(btScalar(delta.asSeconds()), 8, btScalar(timestep.sec()));
    if (deferredRenderer) {
        updater->operator()(scene); 
    }
}

void loop(sf::Time const& delta) {
// FIXME: Should we render first or update physics first?  By rendering first,
// we get to update physics in parallel before the vsync.  However, if we do
// this, the rendering will be out-of-date relative to realtime due to some
// elapsed time between physics update for prev. frame & rendering for the
// current frame.
    render(delta); // Render
    input(delta); // Process input 
    physics(delta); // Physics in parallel
    if (window) {
        window->display();  // Wait for vsync
    }
}

void exit() {
}

void tick() {
// Wait for the physics step event
    tickEvent->wait();
}

void render() {
// Wait for a new frame
    renderEvent->wait();
}

void input() {
// Block the current coroutine until there is input.
    inputEvent->wait();
}


void run() {
    init();
    auto cloop = coro::start(std::bind(task, loop, 120));
    //coro::start(std::bind(task, sync, 60));
    // Run at 60 Hz for better response time
    coro::run();
}

void tickListenerIs(TickListener* listener) {
    tickListener.push_back(listener);
}

void tickListenerDel(TickListener* listener) {
    std::remove(tickListener.begin(), tickListener.end(), listener);
}

void renderListenerIs(RenderListener* listener) {
    renderListener.push_back(listener);
}

void renderListenerDel(RenderListener* listener) {
    std::remove(renderListener.begin(), renderListener.end(), listener);
}

}
