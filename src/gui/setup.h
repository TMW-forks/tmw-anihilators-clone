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

#ifndef SETUP_H
#define SETUP_H

#include <list>

#include <guichan/actionlistener.hpp>

#include "window.h"

#include "../guichanfwd.h"

class SetupTab;
class Button;

/**
 * The setup dialog.
 *
 * \ingroup GUI
 */
class Setup : public Window, public gcn::ActionListener
{
    public:
        /**
         * Constructor.
         */
        Setup();

        /**
         * Destructor.
         */
        ~Setup();

        void setInGame(bool inGame);

        /**
         * Event handling method.
         */
        void action(const gcn::ActionEvent &event);

    private:
        std::list<SetupTab*> mTabs;
        gcn::Button *mResetWindows;
};

#endif
