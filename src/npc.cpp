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

#include "npc.h"

#include "animatedsprite.h"

#include "net/messageout.h"
#include "net/protocol.h"

class Spriteset;
extern Spriteset *npcset;

NPC *current_npc = 0;

NPC::NPC(Uint16 id, Uint16 job, Map *map):
    Being(id, job, map)
{
    mSprites[BASE_SPRITE] = new AnimatedSprite("graphics/sprites/npc.xml", job-100);
}

Being::Type
NPC::getType() const
{
    return Being::NPC;
}

void
NPC::talk()
{
    MessageOut outMsg(CMSG_NPC_TALK);
    outMsg.writeLong(mId);
    outMsg.writeByte(0);
    current_npc = this;
}

void
NPC::nextDialog()
{
    MessageOut outMsg(CMSG_NPC_NEXT_REQUEST);
    outMsg.writeLong(mId);
}

void
NPC::dialogChoice(char choice)
{
    MessageOut outMsg(CMSG_NPC_LIST_CHOICE);
    outMsg.writeLong(mId);
    outMsg.writeByte(choice);
}

/*
 * TODO Unify the buy() and sell() methods, without sacrificing readability of
 * the code calling the method. buy(bool buySell) would be bad...
 */
void
NPC::buy()
{
    MessageOut outMsg(CMSG_NPC_BUY_SELL_REQUEST);
    outMsg.writeLong(mId);
    outMsg.writeByte(0);
}

void
NPC::sell()
{
    MessageOut outMsg(CMSG_NPC_BUY_SELL_REQUEST);
    outMsg.writeLong(mId);
    outMsg.writeByte(1);
}
