/*
 *  The Mana World
 *  Copyright (C) 2004  The Mana World Development Team
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

#ifndef NET_EA_ADMINHANDLER_H
#define NET_EA_ADMINHANDLER_H

#include "net/messagehandler.h"
#include "net/net.h"

class AdminHandler : public MessageHandler, public Net::AdminHandler
{
    public:
        AdminHandler();

        virtual void handleMessage(MessageIn &msg);

        virtual void announce(const std::string &text);

        virtual void localAnnounce(const std::string &text);

        virtual void hide(bool hide);

        virtual void kick(int playerId);

        virtual void kick(const std::string &name);

        virtual void ban(int playerId);

        virtual void ban(const std::string &name);

        virtual void unban(int playerId);

        virtual void unban(const std::string &name);

        virtual void mute(int playerId, int type, int limit);
};

extern AdminHandler *adminHandler;

#endif // NET_EA_ADMINHANDLER_H
