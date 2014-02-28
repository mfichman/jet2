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

#pragma once

#include "jet2/Common.hpp"
#include "jet2/Object.hpp"

namespace jet2 {

typedef std::function<void()> MenuFunc;

class Menu : public Object {
public:
    Menu(const std::string& title, int titleSize=86);
    ~Menu();

    void optionIs(std::string const& text, MenuFunc func); 
    void optionSizeIs(int optionSize) { optionSize_ = optionSize; }

    int titleSize() const { return titleSize_; }
    int optionSize() const { return optionSize_; }
    Ptr<sfr::Ui> ui() const { return ui_; }

    // Blocks the current coroutine until a menu option is selected
    void select();
    void quit();
    
private:
    void input();
    void mouseButtonPressed(sf::Event const& evt);
    void mouseMoved(sf::Event const& evt);

    Ptr<sfr::Ui> ui_;
    Ptr<InputDispatcher> dispatcher_;
    std::vector<MenuFunc> func_;
    int option_ = 0;
    int offset_ = 0;


    bool quit_ = false;
    int verticalSpacing_ = 15;
    int optionSize_ = 32;
    int titleSize_;
    std::string fontName_ = "NeuropolXBold";
};


}
