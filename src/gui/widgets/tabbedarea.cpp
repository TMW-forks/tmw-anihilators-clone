/*
 *  The Mana World
 *  Copyright 2008 The Mana World Development Team
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
 *  $Id:  $
 */

#include "tabbedarea.h"

#include <guichan/widgets/container.hpp>

TabbedArea::TabbedArea() : gcn::TabbedArea()
{
    mWidgetContainer->setOpaque(false);
}

int TabbedArea::getNumberOfTabs()
{
    return mTabs.size();
}

gcn::Tab* TabbedArea::getTab(const std::string &name)
{
    std::vector< std::pair<gcn::Tab*, gcn::Widget*> >::iterator itr = mTabs.begin(),
                                                        itr_end = mTabs.end();
    while (itr != itr_end)
    {
        if ((*itr).first->getCaption() == name)
        {
            return (*itr).first;
        }
        ++itr;
    }
}

void TabbedArea::draw(gcn::Graphics *graphics)
{
    if (mTabs.empty())
    {
        return;
    }

    unsigned int i;
    for (i = 0; i < mTabs.size(); i++)
    {
        if (mTabs[i].first == mSelectedTab)
        {
            mTabs[i].second->setWidth(getWidth());
            mTabs[i].second->setHeight(getHeight());
            mTabs[i].second->logic();
            break;
        }
    }

    gcn::TabbedArea::draw(graphics);
}

gcn::Widget* TabbedArea::getWidget(const std::string &name)
{
    unsigned int i;
    for (i = 0; i < mTabs.size(); i++)
    {
        if (mTabs[i].first->getCaption() == name)
        {
            return mTabs[i].second;
        }
    }

    return NULL;
}
