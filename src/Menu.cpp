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

#include "jet2/Common.hpp"
#include "jet2/Menu.hpp"
#include "jet2/Kernel.hpp"
#include "jet2/InputDispatcher.hpp"
#include "jet2/Functions.hpp"

namespace jet2 {

Menu::Menu(const std::string& title, GLfloat titleSize) {
    dispatcher_.reset(new InputDispatcher);
    titleSize_ = titleSize;

    ui_ = jet2::scene->ui()->childIs<sfr::Ui>();
    ui_->xIs(sfr::Coord(1, sfr::Coord::PERCENT));
    ui_->yIs(sfr::Coord(.25, sfr::Coord::PERCENT));

    offset_ = 0;

    auto ui = ui_->childIs<sfr::Ui>();
    ui->yIs(sfr::Coord(offset_, sfr::Coord::PIXELS, sfr::Coord::BEGIN));
    ui->xIs(sfr::Coord(0, sfr::Coord::PERCENT));
    ui->heightIs(sfr::Span(titleSize_+verticalSpacing_*2.0f, sfr::Span::PIXELS));

    auto caps = title;
    std::transform(caps.begin(), caps.end(), caps.begin(), ::toupper);

    auto font = format("fonts/%s.ttf#%d", fontName_.c_str(), int(titleSize_));
    titleText_ = ui->childIs<sfr::Text>();
    titleText_->textIs(caps);
    titleText_->fontIs(assets->assetIs<sfr::Font>(font));
    titleText_->colorIs(sfr::Color(1.0f, .870f, .361f, 1.0f));
    titleText_->sizeIs(titleSize_);
    offset_ += GLfloat(titleText_->size()+verticalSpacing_*2);
}

Menu::~Menu() {
    jet2::scene->ui()->childDel(ui_);
}

void Menu::titleColorIs(sfr::Color const& color) {
    titleText_->colorIs(color);
}

void Menu::optionIs(std::string const& str, MenuFunc func) {
    auto ui = ui_->childIs<sfr::Ui>();
    ui->yIs(sfr::Coord(offset_, sfr::Coord::PIXELS, sfr::Coord::BEGIN));
    ui->xIs(sfr::Coord(0, sfr::Coord::PERCENT));
    ui->heightIs(sfr::Span(GLfloat(optionSize_+verticalSpacing_), sfr::Span::PIXELS));

    //auto font = format("fonts/%s.ttf#%d", fontName_.c_str(), optionSize_);
    auto font = format("fonts/%s.ttf#%d", "NeuropolX", int(optionSize_));
    auto text = ui->childIs<sfr::Text>();
    text->textIs(str);
    text->fontIs(assets->assetIs<sfr::Font>(font));
    text->colorIs(sfr::Color(1.f, 1.f, 1.f, .6f));
    text->sizeIs(optionSize_);
    offset_ += text->size()+verticalSpacing_;
     
    func_.push_back(func);  
}

void Menu::quit() {
    quit_ = true;
}

void Menu::select() {
    quit_ = false;
    while (!quit_) {
        ui_->xIs(sfr::Coord(1, sfr::Coord::PERCENT));
        slide(ui_, sfr::GLvec2(.05f, .25f), coro::Time::sec(.15));

 
        auto mouse = sf::Mouse::getPosition(*jet2::window);
        auto event = sf::Event();
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = mouse.x;
        event.mouseMove.y = mouse.y;
        mouseMoved(event); 
    
        option_ = -1;
        while (option_ == -1) {
            jet2::inputEvent->wait();
            input();
        }
        slide(ui_, sfr::GLvec2(-1, .25), coro::Time::sec(.15));
        func_[option_]();
    }

}

void Menu::input() {
// Check for collisions between mouse click & menu items
    for (auto evt : jet2::inputQueue) {
        switch (evt.type) {
        case sf::Event::MouseButtonPressed:
            mouseButtonPressed(evt);
            break;
        case sf::Event::MouseMoved:
            mouseMoved(evt);
            break;
        default:
            break;
        }
    }
}

void Menu::mouseMoved(sf::Event const& evt) {
    dispatcher_->input(evt, jet2::scene);
    for (GLuint i = 0; i < func_.size(); ++i) {
        auto ui = ui_->child<sfr::Ui>(i+1);
        auto text = ui->child<sfr::Text>(0);
        if (dispatcher_->ui() == ui) {
            text->colorIs(sfr::Color(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            text->colorIs(sfr::Color(1.0f, 1.0f, 1.0f, .6f));
        }
    }
}

void Menu::mouseButtonPressed(sf::Event const& evt) {
// Check for clicks within a button
    dispatcher_->input(evt, jet2::scene);
    for (GLuint i = 0; i < func_.size(); ++i) {
        if (dispatcher_->ui() == ui_->child(i+1)) {
            option_ = i;
            return;
        }
    }
}


}
