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

#include "button.h"

Button::Button(const std::string& caption):
    gcn::Button(caption)
{
    setBorderSize(0);
}

void Button::draw(gcn::Graphics* graphics) {
    int mode;

    if (false /*disabled*/) {
        mode = 3;
    }
    else if (isPressed()) {
        mode = 2;
    }
    else if (hasMouse()) {
        mode = 1;
    }
    else {
        mode = 0;
    }

    int x, y;
    getAbsolutePosition(x, y);

    draw_skinned_rect(gui_bitmap, &gui_skin.button.background[mode],
            x, y, getWidth(), getHeight());

    graphics->setColor(getForegroundColor());

    int textX;
    int textY = getHeight() / 2 - getFont()->getHeight() / 2;

    switch (getAlignment()) {
        case gcn::Graphics::LEFT:
            textX = 4;
            break;
        case gcn::Graphics::CENTER:
            textX = getWidth() / 2;
            break;
        case gcn::Graphics::RIGHT:
            textX = getWidth() - 4;
            break;
        default:
            throw GCN_EXCEPTION("Button::draw. Uknown alignment.");
    }

    graphics->setFont(getFont());

    if (isPressed()) {
        graphics->drawText(getCaption(), textX + 1, textY + 1, getAlignment());
    }
    else {
        graphics->drawText(getCaption(), textX, textY, getAlignment());
    }    
}
