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
 *  $Id$
 */

#ifndef _TMW_GUI_GUILDWINDOW_H
#define _TMW_GUI_GUILDWINDOW_H

#include <iosfwd>
#include <map>
#include <vector>

#include <guichan/actionlistener.hpp>
#include <guichan/widgets/listbox.hpp>

#include "window.h"

#include "../guichanfwd.h"

class LocalPlayer;
class TextDialog;
class ConfirmDialog;
class GuildListBox;
class ScrollArea;
class GCContainer;
class TabbedArea;

class GuildWindow : public Window, public gcn::ActionListener
{
public:
    /**
     * Constructor.
     */
    GuildWindow();

    /**
     * Destructor.
     */
    ~GuildWindow();

    /**
     * Called when receiving actions from widget.
     */
    void action(const gcn::ActionEvent &event);

    /**
     * Draw this window.
     */
    void draw(gcn::Graphics *graphics);

    /**
     * Updates this dialog.
     */
    void update();

    /**
     * Create a new tab for a guild list.
     */
    void newGuildTab(const std::string &guildName);

    /**
     * Display guild's member list to active tab
     */
    void setTab(const std::string &guildName);

    /**
     * Update the contents of the active tab
     */
    void updateTab();

    /**
     * Check if the window is in focus
     */
    bool isFocused();

    /**
     * Create a dialog for accepting an invite
     */
    void openAcceptDialog(const std::string &inviterName, const std::string &guildName);

    /**
     * Request member list
     */
    void requestMemberList(short guildId);

    /**
     * Removes the selected tab
     */
    void removeTab(int guildId);

    /**
     * Set guild member status in userlist
     */
    void setOnline(const std::string &guildName, const std::string &member,
                   bool online);

protected:
    /**
     * Get selected guild tab
     * @return Returns selected guild
     */
    short getSelectedGuild();

private:
    gcn::Button *mGuildButton[3];
    TextDialog *guildDialog;
    TextDialog *inviteDialog;
    ConfirmDialog *acceptDialog;
    TabbedArea *mGuildTabs;
    ScrollArea *mScrollArea;
    bool mFocus;
    std::string invitedGuild;
    typedef std::map<std::string, GuildListBox*> GuildListMap;
    GuildListMap mGuildLists;
};

extern GuildWindow *guildWindow;

#endif
