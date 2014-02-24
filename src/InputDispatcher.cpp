/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "jet2/Common.hpp"
#include "jet2/InputDispatcher.hpp"

using namespace jet2;

void InputDispatcher::input(sf::Event const& event, Ptr<sfr::World> world) {
    event_ = event;
    operator()(world);
}

void InputDispatcher::operator()(Ptr<sfr::World> world) {
    world_ = world;
    rect_.x = 0;
    rect_.y = 0;
    rect_.width = world->camera()->viewportWidth();
    rect_.height = world->camera()->viewportHeight();
    operator()(world->ui());
}

void InputDispatcher::operator()(Ptr<sfr::Ui> ui) {
    sfr::Rect parentRect = rect_;
    rect_.width = ui->width().absolute(parentRect.width);
    rect_.height = ui->height().absolute(parentRect.height);
    rect_.x = ui->x().absolute(parentRect.x, parentRect.width, rect_.width);
    rect_.y = ui->y().absolute(parentRect.y, parentRect.height, rect_.height);

    if (event_.mouseButton.x < rect_.x || event_.mouseButton.x > (rect_.x+rect_.width)) {
        return;
    } else if (event_.mouseButton.y < rect_.y || event_.mouseButton.y > (rect_.y+rect_.height)) {
        return;
    }

    for(sfr::Iterator<std::vector<Ptr<sfr::Node>>> i = ui->children(); i; i++) {
        i->get()->operator()(std::static_pointer_cast<InputDispatcher>(shared_from_this()));
    }

    rect_ = parentRect;
}


