/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 */

#include "window.h"
#include "gui.h"
#include <guichan/allegro.hpp>
#include "../resources/resourcemanager.h"

WindowContainer *Window::windowContainer = NULL;

Window::Window(const std::string& text, bool modal, Window *parent):
    parent(parent),
    caption(text),
    mousePX(0),
    mousePY(0),
    snapSize(8),
    mouseDown(false),
    modal(modal),
    titlebarHeight(20)
{
#ifdef __DEBUG
    std::cout << "Window::Window(\"" << caption << "\")\n";
#endif

    titlebarColor.r = 203;
    titlebarColor.g = 203;
    titlebarColor.b = 203;

    setBaseColor(gcn::Color(255, 255, 255));

    // Load dialog title bar image
    ResourceManager *resman = ResourceManager::getInstance();
    dLeft = resman->createImage("Skin/dialogLeft.bmp");
    dMid = resman->createImage("Skin/dialogMiddle.bmp");
    dRight = resman->createImage("Skin/dialogRight.bmp");

    // Register mouse listener
    addMouseListener(this);

    // Add chrome
    chrome = new gcn::Container();
    chrome->setOpaque(false);
    chrome->setY(titlebarHeight);
    gcn::Container::add(chrome);

    // Add this window to the window container
    if (windowContainer) {
        windowContainer->add(this, modal);
    }
    else {
        throw GCN_EXCEPTION("Window::Window. no windowContainer set");
    }
}

Window::~Window()
{
#ifdef __DEBUG
    std::cout << "Window::~Window(\"" << caption << "\")\n";
#endif

    // Free dialog bitmaps
    //release_bitmap(dLeft);
    //release_bitmap(dMid);
    //release_bitmap(dRight);

    delete chrome;
}

void Window::setWindowContainer(WindowContainer *wc)
{
    windowContainer = wc;
}

void Window::draw(gcn::Graphics* graphics)
{
    // Draw container background when opaque
    if (mOpaque) {
        graphics->setColor(getBaseColor());
        graphics->fillRectangle(gcn::Rectangle(0, titlebarHeight,
                    getDimension().width,
                    getDimension().height - titlebarHeight));
    }

    // Draw line around window
    graphics->setColor(titlebarColor);
    graphics->drawRectangle(gcn::Rectangle(0, titlebarHeight - 1,
                getWidth(), getHeight() - titlebarHeight + 1));

    // Skinned dialog render
    if (typeid(*graphics) == typeid(gcn::AllegroGraphics))
    {
        gcn::AllegroGraphics *gfx = (gcn::AllegroGraphics*)graphics;
        BITMAP *screen = gfx->getTarget();
        int x, y;
        getAbsolutePosition(x, y);

        // Draw title bar
        dLeft->draw(screen, x, y);
        dMid->drawPattern(screen,
                x + dLeft->getWidth(), y,
                getWidth() - dLeft->getWidth() - dRight->getWidth(),
                dMid->getHeight());
        dRight->draw(screen, x + getWidth() - dRight->getWidth(), y);
    }
    else {
        // Plain title bar
        graphics->setColor(titlebarColor);
        graphics->fillRectangle(gcn::Rectangle(0, 0,
                    getWidth(), titlebarHeight));
    }

    // Draw title
    graphics->setFont(getFont());
    graphics->drawText(caption, 4, 4, gcn::Graphics::LEFT);

    drawChildren(graphics);
}

void Window::setTitle(const std::string& text)
{
    caption = std::string(text);
}

void Window::setDimension(const gcn::Rectangle &dimension)
{
    gcn::Container::setDimension(gcn::Rectangle(
                dimension.x,
                dimension.y - titlebarHeight,
                dimension.width,
                dimension.height + titlebarHeight));
    chrome->setDimension(gcn::Rectangle(0, titlebarHeight,
                dimension.width, dimension.height));
}

void Window::setWidth(int width)
{
    gcn::Container::setWidth(width);
    chrome->setWidth(width);
}

void Window::setHeight(int height)
{
    gcn::Container::setHeight(height + titlebarHeight);
    chrome->setHeight(height);
}

void Window::setLocationRelativeTo(gcn::Widget* widget)
{
    int wx, wy;
    int x, y;

    widget->getAbsolutePosition(wx, wy);
    getAbsolutePosition(x, y);

    setPosition(
            getX() + (wx + (widget->getWidth() - getWidth()) / 2 - x),
            getY() + (wy + (widget->getHeight() - getHeight()) / 2 - y));
}

void Window::setSize(int width, int height)
{
    setWidth(width);
    setHeight(height);
}

Window *Window::getParentWindow()
{
    return parent;
}

bool Window::isModal()
{
    return modal;
}

void Window::add(gcn::Widget *w)
{
    chrome->add(w);
}

void Window::add(Widget *w, int x, int y)
{
    chrome->add(w, x, y);
}

void Window::mousePress(int mx, int my, int button)
{
    mouseDown = true;

    mousePX = mx;
    mousePY = my;
}

void Window::mouseRelease(int mx, int my, int button)
{
    mouseDown = false;
}

void Window::mouseMotion(int mx, int my)
{
    if (mouseDown)
    {
        int winWidth = this->getDimension().width;
        int winHeight = this->getDimension().height;
        int x = this->getDimension().x;
        int y = this->getDimension().y;

        x = x - (mousePX - mx);
        y = y - (mousePY - my);

        // Keep guichan window inside window
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + winWidth > 799) x = 799 - winWidth;
        if (y + winHeight > 599) y = 599 - winHeight;

        // Snap window to edges
        if (x < snapSize) x = 0;
        if (y < snapSize) y = 0;
        if (x + winWidth + snapSize > 799) x = 799 - winWidth;
        if (y + winHeight + snapSize > 599) y = 599 - winHeight;

        this->setPosition(x, y);
    }
}

void Window::mouseOut()
{
    mouseDown = false;
}

void Window::_mouseInputMessage(const gcn::MouseInput &mouseInput)
{
    if (mouseInput.getType() == gcn::MouseInput::MOTION && mouseDown) {
        // It's a window drag event
        gcn::Widget::_mouseInputMessage(mouseInput);
    }
    else {
        // It's something else
        gcn::Container::_mouseInputMessage(mouseInput);
    }
}
