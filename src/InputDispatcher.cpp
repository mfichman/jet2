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
    ui_ = 0;
    event_ = event;
    operator()(world);
}

void InputDispatcher::operator()(Ptr<sfr::World> world) {
    world_ = world;
    rect_.x = 0;
    rect_.y = 0;
    rect_.width = GLfloat(world->camera()->viewportWidth());
    rect_.height = GLfloat(world->camera()->viewportHeight());
    operator()(world->ui());
}

bool isInRect(sfr::Rect const& rect, GLuint x, GLuint y) {
    if (x < rect.x || x > (rect.x+rect.width)) {
        return false;
    } else if (y < rect.y || y > (rect.y+rect.height)) {
        return false;
    } 
    return true;
}

void InputDispatcher::operator()(Ptr<sfr::Ui> ui) {
    sfr::Rect parentRect = rect_;
    rect_.width = ui->width().absolute(parentRect.width);
    rect_.height = ui->height().absolute(parentRect.height);

    rect_.x = ui->x().absolute(parentRect.x, parentRect.width, rect_.width);
    rect_.y = ui->y().absolute(parentRect.y, parentRect.height, rect_.height);

    if (event_.type == sf::Event::MouseMoved) {
        if (!isInRect(rect_, event_.mouseMove.x, event_.mouseMove.y)) {
            rect_ = parentRect;
            return;
        }
    } else if (event_.type == sf::Event::MouseButtonPressed) {
        if (!isInRect(rect_, event_.mouseButton.x, event_.mouseButton.y)) {
            rect_ = parentRect;
            return;
        }
    } else {
        rect_ = parentRect;
        return;
    }
    
    ui_ = ui;

    for(sfr::Iterator<std::vector<Ptr<sfr::Node>>> i = ui->children(); i; i++) {
        i->get()->operator()(std::static_pointer_cast<InputDispatcher>(shared_from_this()));
    }

    rect_ = parentRect;
}


