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

#ifndef _TMW_LOCALPLAYER_H
#define _TMW_LOCALPLAYER_H

#include <memory.h>

#include "player.h"

// TODO move into some sane place...
#define MAX_SLOT 2

class FloorItem;
class Inventory;
class Item;

class LocalPlayer : public Player
{
    public:
        enum Attribute {
            STR = 0, AGI, VIT, INT, DEX, LUK
        };

        LocalPlayer();

        virtual ~LocalPlayer();

        virtual void logic();
        virtual void nextStep();

        /**
         * Draws the name text below the being.
         */
        virtual void
        drawName(Graphics *graphics, Sint32 offsetX, Sint32 offsetY) {};

        virtual Type getType() const;

        void clearInventory();
        void addInvItem(int id, int quantity, bool equipment);
        void addInvItem(int index, int id, int quantity, bool equipment);
        Item* getInvItem(int index);

        /**
         * Equips an item.
         */
        void equipItem(Item *item);

        /**
         * Unequips an item.
         */
        void unequipItem(Item *item);

        void useItem(Item *item);
        void dropItem(Item *item, int quantity);
        void pickUp(FloorItem *item);

        /**
         * Sents a trade request to the given being.
         */
        void trade(Being *being) const;

        /**
         * Accept or decline a trade offer
         */
        void tradeReply(bool accept);

        /**
         * Returns true when the player is ready to accept a trade offer.
         * Returns false otherwise.
         */
        bool tradeRequestOk() const;

        /**
         * Sets the trading state of the player, i.e. whether or not he is
         * currently involved into some trade.
         */
        void setTrading(bool trading) { mTrading = trading; }

        void attack(Being *target=NULL, bool keep=false);
        void stopAttack();
        Being* getTarget() const;

        /**
         * Sets the target being of the player.
         */
        void setTarget(Being* target) { mTarget = target; }

        void walk(unsigned char dir);

        /**
         * Sets a new destination for this being to walk to.
         */
        void setDestination(Uint16 x, Uint16 y);

        void raiseAttribute(Attribute attr);
        void raiseSkill(Uint16 skillId);

        void toggleSit();
        void emote(Uint8 emotion);

        void revive();

        Uint32 mCharId;

        Uint32 mXp, mJobXp;
        Uint16 mLevel;
        Uint32 mJobLevel;
        Uint32 mXpForNextLevel, mJobXpForNextLevel;
        Uint16 mHp, mMaxHp, mMp, mMaxMp;
        Uint32 mMoney;

        Uint32 mTotalWeight, mMaxWeight;

        Uint8 mAttr[6];
        Uint8 mAttrUp[6];

        Sint16 ATK, MATK, DEF, MDEF, HIT, FLEE;
        Sint16 ATK_BONUS, MATK_BONUS, DEF_BONUS, MDEF_BONUS, FLEE_BONUS;

        Uint16 mStatPoint, mSkillPoint;
        Uint16 mStatsPointsToAttribute;

        float mLastAttackTime; /**< Used to synchronize the charge dialog */

        std::auto_ptr<Inventory> mInventory;

    protected:
        Being *mTarget;
        FloorItem *mPickUpTarget;

        bool mTrading;
        int mLastAction;    /**< Time stamp of the last action, -1 if none */
};

extern LocalPlayer *player_node;

#endif
