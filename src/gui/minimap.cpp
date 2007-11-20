/*
 *  The Mana World
 *  Copyright 2004-2005 The Mana World Development Team
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

#include "minimap.h"

#include "../being.h"
#include "../beingmanager.h"
#include "../graphics.h"
#include "../localplayer.h"

#include "../resources/image.h"

Minimap::Minimap():
    Window("Map"),
    mMapImage(NULL)
{
    setWindowName("MiniMap");
    setDefaultSize(5, 25, 100, 100);
    loadWindowState();
}

Minimap::~Minimap()
{
    if (mMapImage)
    {
        mMapImage->decRef();
    }
}

void Minimap::setMapImage(Image *img)
{
    if (mMapImage)
    {
        mMapImage->decRef();
    }

    mMapImage = img;

    if (mMapImage)
    {
        setVisible(true);
        mMapImage->setAlpha(0.7);
    }
    else
    {
        setVisible(false);
    }
}

void Minimap::draw(gcn::Graphics *graphics)
{
    Window::draw(graphics);

    if (mMapImage != NULL)
    {
        static_cast<Graphics*>(graphics)->
            drawImage(mMapImage, getPadding(), getTitleBarHeight());
    }

    Beings &beings = beingManager->getAll();
    BeingIterator bi;

    for (bi = beings.begin(); bi != beings.end(); bi++)
    {
        Being *being = (*bi);
        int dotSize = 2;

        switch (being->getType()) {
            case Being::PLAYER:
                if (being == player_node)
                {
                    dotSize = 3;
                    graphics->setColor(gcn::Color(61, 209, 52));
                    break;
                }
                graphics->setColor(gcn::Color(61, 52, 209));
                break;

            case Being::MONSTER:
                graphics->setColor(gcn::Color(209, 52, 61));
                break;

            default:
                break;
        }

        int offset = (dotSize - 1) / 2;

        graphics->fillRectangle(gcn::Rectangle(
                    being->mX / 2 + getPadding() - offset,
                    being->mY / 2 + getTitleBarHeight() - offset,
                    dotSize, dotSize));
    }
}
