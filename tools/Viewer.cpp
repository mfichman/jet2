/*
 * Copyright (c) 2014 Matt Fichman
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

#include <jet2/Common.hpp>
#include <jet2/jet2.hpp>

using namespace sfr;


void update(Ptr<sfr::Transform> cameraNode, Ptr<sfr::Transform> node) {
    auto root = jet2::scene->root();

    auto radiansX = 0.0f;
    auto radiansY = 0.0f;
    auto x = 0;
    auto y = 0;
    auto zoom = 8.f;
    auto up = sfr::Vector(0., 1., 0.);

    bool pressed = false;
    for (;;) {
        for (auto event : jet2::inputQueue) {
            if (event.type == sf::Event::MouseWheelMoved) {
                zoom += event.mouseWheel.delta * .1f;
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Add) {
                    zoom -= 3.f;
                } else if (event.key.code == sf::Keyboard::Subtract) {
                    zoom += 3.f;
                }
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mouse = sf::Mouse::getPosition(*jet2::window);
            if (!pressed) {
                x = mouse.x;
                y = mouse.y;
            }
            radiansX += float((x-mouse.x)*jet2::timestep.sec()); 
            radiansY += float((y-mouse.y)*jet2::timestep.sec());
            pressed = true;
            x = mouse.x;
            y = mouse.y;
        } else {
            pressed = false;
        }
        auto rotateX = sfr::Matrix(sfr::Quaternion(sfr::Vector(0, 1.0f, 0), radiansX), sfr::Vector(0, 0, 0));
        auto rotateY = sfr::Matrix(sfr::Quaternion(sfr::Vector(0, 0, 1.0f), radiansY), sfr::Vector(0, 0, 0));
        auto transform = rotateY * rotateX;// * look;
        //auto transform = rotateX * look;
        node->transformIs(transform);

        auto position = sfr::Vector(0, 0, zoom);
        cameraNode->transformIs(sfr::Matrix::look(position, sfr::Vector(), up));

        jet2::tick();
    }
}

int main(int argc, char** argv) {
    jet2::init();

    auto root = jet2::scene->root();

    //auto position = sfr::Vector(2.5, 2, 2.5);
    auto position = sfr::Vector(8, 8, 8);
    auto up = sfr::Vector(0., 1., 0.);

    auto cameraNode = root->childIs<sfr::Transform>("camera");
    cameraNode->transformIs(sfr::Matrix::look(position, sfr::Vector(), up));

    auto camera = cameraNode->childIs<sfr::Camera>();
    camera->nearIs(0.1f);
    camera->farIs(1000);
    camera->viewportWidthIs(jet2::window->getSize().x);
    camera->viewportHeightIs(jet2::window->getSize().y);
    jet2::scene->cameraIs(camera);

    auto spotNode = root->childIs<sfr::Transform>("spot");
    spotNode->positionIs(sfr::Vector(0, 16, 0));

    auto spot = spotNode->childIs<sfr::SpotLight>();
    spot->spotCutoffIs(20.f);
    spot->spotPowerIs(40.f);
	spot->constantAttenuationIs(1.f);
    spot->linearAttenuationIs(0.f);
	spot->quadraticAttenuationIs(0.f);
    spot->specularColorIs(sfr::Color(3.f, 3.f, 3.f, 1.f));
	spot->diffuseColorIs(sfr::Color(1.5f, 1.5f, 1.5f, 1.f));
    spot->directionIs(sfr::Vector(0, -1, 0));
    spot->shadowMapIs(std::make_shared<sfr::DepthRenderTarget>(2048, 2048));

    auto hemi = root->childIs<sfr::HemiLight>();
    hemi->constantAttenuationIs(1);
    hemi->linearAttenuationIs(0);
    hemi->quadraticAttenuationIs(0);
    hemi->diffuseColorIs(sfr::Color(1.f, 1.f, 1.f, 1.f));
    hemi->backDiffuseColorIs(sfr::Color(0.8f, 0.8f, 0.8f, 1.f));
    hemi->specularColorIs(sfr::Color(1.f, 1., 1.f, 1.f));
    hemi->directionIs(sfr::Vector(0.f, 0.f, -1.f));

    auto plane = root->childIs<sfr::Transform>("plane");
/*
    plane->childIs(jet2::assets->assetIs<sfr::Transform>("meshes/Plane.obj"));
    plane->positionIs(sfr::Vector(0, -1, 0));
*/

    auto asset = jet2::assets->assetIs<sfr::Transform>(argv[1]);
    root->childIs(asset);

    auto input = coro::start(std::bind(update, cameraNode, asset));

    jet2::run();
    jet2::exit();

    return 0;
}
