/*
 *  Aethyra
 *  Copyright (C) 2004  The Mana World Development Team
 *
 *  This file is part of Aethyra based on original code
 *  from The Mana World.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LISTBOX_H
#define LISTBOX_H

#include <guichan/widgets/listbox.hpp>

class SelectionListener;

/**
 * A list box, meant to be used inside a scroll area. Same as the Guichan list
 * box except this one doesn't have a background, instead completely relying
 * on the scroll area. It also adds selection listener functionality.
 *
 * \ingroup GUI
 */
class ListBox : public gcn::ListBox
{
    public:
        /**
         * Constructor.
         */
        ListBox(gcn::ListModel *listModel);

        /**
         * Draws the list box.
         */
        void draw(gcn::Graphics *graphics);

        void mouseDragged(gcn::MouseEvent &event);

        // Inherited from KeyListener

        virtual void keyPressed(gcn::KeyEvent& keyEvent);

        // Inherited from MouseListener

        virtual void mouseWheelMovedUp(gcn::MouseEvent& mouseEvent);

        virtual void mouseWheelMovedDown(gcn::MouseEvent& mouseEvent);

         /**
         * Sets the selected item. The selected item is represented by
         * an index from the list model.
         *
         * @param selected the selected item as an index from the list model.
         * @see getSelected
         */
        void setSelected(int selected);

    private:
        static float mAlpha;
};

#endif
