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

#include <guichan/widgets/label.hpp>

#include "whispertab.h"

#include "../../beingmanager.h"
#include "../../commandhandler.h"
#include "../../channel.h"
#include "../../configuration.h"
#include "../../game.h"
#include "../../localplayer.h"

#ifdef TMWSERV_SUPPORT
#include "../../net/tmwserv/chatserver/chatserver.h"
#include "../../net/tmwserv/gameserver/player.h"
#else
#include "../../party.h"
#include "../../net/messageout.h"
#include "../../net/ea/protocol.h"
#endif

#include "../../resources/iteminfo.h"
#include "../../resources/itemdb.h"

#include "../../utils/dtor.h"
#include "../../utils/gettext.h"
#include "../../utils/strprintf.h"
#include "../../utils/stringutils.h"

WhisperTab::WhisperTab(const std::string &nick) :
    ChatTab(nick),
    mNick(nick)
{
}

WhisperTab::~WhisperTab()
{
}

void WhisperTab::sendChat(std::string &msg) {
    if (msg.length() == 0) {
        chatLog(_("Cannot send empty chat!"), BY_SERVER, false);
        return;
    }

#ifdef TMWSERV_SUPPORT
    Net::ChatServer::privMsg(mNick, msg);
#else
    MessageOut outMsg(chatWindow->mNetwork);
    outMsg.writeInt16(CMSG_CHAT_WHISPER);
    outMsg.writeInt16(msg.length() + 28);
    outMsg.writeString(mNick, 24);
    outMsg.writeString(msg, msg.length());
#endif

    chatLog(strprintf(_("%s: %s"), player_node->getName().c_str(),
                        msg.c_str()), BY_PLAYER, false);
}
