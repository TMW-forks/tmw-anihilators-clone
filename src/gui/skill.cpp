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

#include <algorithm>

#include <guichan/widgets/label.hpp>

#include "skill.h"

#include "button.h"
#include "listbox.h"
#include "scrollarea.h"
#include "windowcontainer.h"

#include "../localplayer.h"

#include "../utils/dtor.h"

const char *skill_db[] = {
    // 0-99
    "", "Basic", "Sword", "Two hand", "HP regeneration", "Bash", "Provoke", "Magnum", "Endure", "MP regeneration",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "MAX weight", "Discount", "Overcharge", "",
    "Identify", "", "", "", "", "", "", "", "Double", "Miss",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    // 100-199
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "First aid", "Play as dead", "Moving recovery", "Fatal blow", "Auto berserk", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
};


SkillDialog::SkillDialog():
    Window("Skills")
{
    setWindowName("Skills");
    setCloseButton(true);
    setDefaultSize(windowContainer->getWidth() - 255, 25, 240, 240);

    mSkillListBox = new ListBox(this);
    ScrollArea *skillScrollArea = new ScrollArea(mSkillListBox);
    mPointsLabel = new gcn::Label("Skill Points:");
    mIncButton = new Button("Up", "inc", this);
    mUseButton = new Button("Use", "use", this);
    mUseButton->setEnabled(false);

    mSkillListBox->setActionEventId("skill");

    skillScrollArea->setHorizontalScrollPolicy(gcn::ScrollArea::SHOW_NEVER);
    skillScrollArea->setDimension(gcn::Rectangle(5, 5, 230, 180));
    mPointsLabel->setDimension(gcn::Rectangle(8, 190, 200, 16));
    mIncButton->setPosition(skillScrollArea->getX(), 210);
    mUseButton->setPosition(mIncButton->getX() + mIncButton->getWidth() + 5,
        210);

    add(skillScrollArea);
    add(mPointsLabel);
    add(mIncButton);
    add(mUseButton);

    mSkillListBox->addActionListener(this);

    setLocationRelativeTo(getParent());
    loadWindowState();
}

SkillDialog::~SkillDialog()
{
    cleanList();
}

void SkillDialog::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "inc")
    {
        // Increment skill
        int selectedSkill = mSkillListBox->getSelected();
        if (selectedSkill >= 0)
        {
            player_node->raiseSkill(mSkillList[selectedSkill]->id);
        }
    }
    else if (event.getId() == "skill")
    {
        mIncButton->setEnabled(
                mSkillListBox->getSelected() > -1 &&
                player_node->mSkillPoint > 0);
    }
    else if (event.getId() == "close")
    {
        setVisible(false);
    }
}

void SkillDialog::update()
{
    if (mPointsLabel != NULL) {
        char tmp[128];
        sprintf(tmp, "Skill points: %i", player_node->mSkillPoint);
        mPointsLabel->setCaption(tmp);
    }

    mIncButton->setEnabled(mSkillListBox->getSelected() > -1 &&
            player_node->mSkillPoint > 0);
}

int SkillDialog::getNumberOfElements()
{
    return mSkillList.size();
}

std::string SkillDialog::getElementAt(int i)
{
    if (i >= 0 && i < (int)mSkillList.size())
    {
        char tmp[128];
        sprintf(tmp, "%s    Lv: %i    Sp: %i",
                skill_db[mSkillList[i]->id],
                mSkillList[i]->lv,
                mSkillList[i]->sp);
        return tmp;
    }
    return "";
}

bool SkillDialog::hasSkill(int id)
{
    for (unsigned int i = 0; i < mSkillList.size(); i++) {
        if (mSkillList[i]->id == id) {
            return true;
        }
    }
    return false;
}

void SkillDialog::addSkill(int id, int lvl, int mp)
{
    SKILL *tmp = new SKILL();
    tmp->id = id;
    tmp->lv = lvl;
    tmp->sp = mp;
    mSkillList.push_back(tmp);
}

void SkillDialog::setSkill(int id, int lvl, int mp)
{
    for (unsigned int i = 0; i < mSkillList.size(); i++) {
        if (mSkillList[i]->id == id) {
            mSkillList[i]->lv = lvl;
            mSkillList[i]->sp = mp;
        }
    }
}

void SkillDialog::cleanList()
{
    for_each(mSkillList.begin(), mSkillList.end(), make_dtor(mSkillList));
    mSkillList.clear();
}
