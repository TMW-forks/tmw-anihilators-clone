/*
 *  The Mana World
 *  Copyright (C) 2007  The Mana World Development Team
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

#include "setup_keyboard.h"

#include <guichan/widgets/label.hpp>
#include <guichan/listmodel.hpp>

#include "button.h"
#include "listbox.h"
#include "ok_dialog.h"
#include "scrollarea.h"

#include "widgets/layouthelper.h"

#include "../configuration.h"
#include "../keyboardconfig.h"

#include "../utils/gettext.h"
#include "../utils/tostring.h"

#include <SDL_keyboard.h>

/**
 * The list model for key function list.
 *
 * \ingroup Interface
 */
class KeyListModel : public gcn::ListModel
{
    public:
        /**
         * Returns the number of elements in container.
         */
        int getNumberOfElements() { return keyboard.KEY_TOTAL; }

        /**
         * Returns element from container.
         */
        std::string getElementAt(int i) { return mKeyFunctions[i]; }

        /**
         * Sets element from container.
         */
        void setElementAt(int i, std::string caption)
        {
            mKeyFunctions[i] = caption;
        }

    private:
        std::string mKeyFunctions[KeyboardConfig::KEY_TOTAL];
};

Setup_Keyboard::Setup_Keyboard():
    mKeyListModel(new KeyListModel()),
    mKeyList(new ListBox(mKeyListModel)),
    mKeySetting(false)
{
    keyboard.setSetupKeyboard(this);
    setOpaque(false);

    refreshKeys();

    mKeyList->setDimension(gcn::Rectangle(0, 0, 185, 140));
    mKeyList->addActionListener(this);
    mKeyList->setSelected(-1);

    ScrollArea *scrollArea = new ScrollArea(mKeyList);

    mAssignKeyButton = new Button(_("Assign"), "assign", this);
    mAssignKeyButton->addActionListener(this);
    mAssignKeyButton->setEnabled(false);

    mMakeDefaultButton = new Button(_("Default"), "makeDefault", this);
    mMakeDefaultButton->addActionListener(this);

    // Do the layout
    LayoutHelper h(this);
    ContainerPlacer place = h.getPlacer(0, 0);

    place(0, 0, scrollArea, 4, 6).setPadding(2);
    place(0, 6, mMakeDefaultButton);
    place(3, 6, mAssignKeyButton);

    setDimension(gcn::Rectangle(0, 0, 250, 200));
}

Setup_Keyboard::~Setup_Keyboard()
{
    delete mKeyList;
    delete mKeyListModel;

    delete mAssignKeyButton;
    delete mMakeDefaultButton;
}

void Setup_Keyboard::apply()
{
    keyUnresolved();

    if (keyboard.hasConflicts())
    {
        new OkDialog(_("Key Conflict(s) Detected."),
                     _("Resolve them, or gameplay may result in strange "
                       "behaviour."));
    }
    keyboard.setEnabled(true);
    keyboard.store();
}

void Setup_Keyboard::cancel()
{
    keyUnresolved();

    keyboard.retrieve();
    keyboard.setEnabled(true);

    refreshKeys();
}

void Setup_Keyboard::action(const gcn::ActionEvent &event)
{
    if (event.getSource() == mKeyList)
    {
        if (!mKeySetting) {
            mAssignKeyButton->setEnabled(true);
        }
    }
    else if (event.getId() == "assign")
    {
        mKeySetting = true;
        mAssignKeyButton->setEnabled(false);
        keyboard.setEnabled(false);
        int i(mKeyList->getSelected());
        keyboard.setNewKeyIndex(i);
        mKeyListModel->setElementAt(i, keyboard.getKeyCaption(i) + ": ?");
    }
    else if (event.getId() == "makeDefault")
    {
        keyboard.makeDefault();
        refreshKeys();
    }
}

void Setup_Keyboard::refreshAssignedKey(int index)
{
    std::string caption;
    char *temp = SDL_GetKeyName(
        (SDLKey) keyboard.getKeyValue(index));
    caption = keyboard.getKeyCaption(index) + ": " + toString(temp);
    mKeyListModel->setElementAt(index, caption);
}

void Setup_Keyboard::newKeyCallback(int index)
{
    mKeySetting = false;
    refreshAssignedKey(index);
    mAssignKeyButton->setEnabled(true);
}

void Setup_Keyboard::refreshKeys()
{
    for (int i = 0; i < keyboard.KEY_TOTAL; i++)
    {
        refreshAssignedKey(i);
    }
}

void Setup_Keyboard::keyUnresolved()
{
    if (mKeySetting) {
        newKeyCallback(keyboard.getNewKeyIndex());
        keyboard.setNewKeyIndex(keyboard.KEY_NO_VALUE);
    }
}
