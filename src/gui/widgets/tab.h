/*
 *  The Mana World
 *  Copyright (C) 2008  The Mana World Development Team
 *
 *  This file is part of The Mana World.
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

#ifndef _TMW_TAB_H
#define _TMW_TAB_H

#include <guichan/widgets/tab.hpp>

class ImageRect;

/**
 * A tab, the same as the guichan tab in 0.8, but extended to allow transparancy
 */
class Tab : public gcn::Tab
{
    public:
        /**
         * Constructor.
         */
        Tab();

        /**
         * Destructor.
         */
        ~Tab();

        /**
         * Draw the tabbed area.
         */
        void draw(gcn::Graphics *graphics);

    private:
        /** Load images if no other instances exist yet */
        void init();

        static ImageRect tabImg[4];    /**< Tab state graphics */
        static int mInstances;         /**< Number of tab instances */
};

#endif


