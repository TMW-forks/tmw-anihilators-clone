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

#include "localplayer.h"

#include "equipment.h"
#include "floor_item.h"
#include "game.h"
#include "inventory.h"
#include "item.h"
#include "main.h"
#include "particle.h"
#include "sound.h"
#include "monster.h"
#include "statuseffect.h"

#include "gui/gui.h"
#include "gui/ministatus.h"

#include "net/messageout.h"
#include "net/protocol.h"

#include "utils/tostring.h"

LocalPlayer *player_node = NULL;

LocalPlayer::LocalPlayer(Uint32 id, Uint16 job, Map *map):
    Player(id, job, map),
    mCharId(0),
    mJobXp(0),
    mLevel(0),
    mJobLevel(0),
    mXpForNextLevel(0), mJobXpForNextLevel(0),
    mHp(0), mMaxHp(0), mMp(0), mMaxMp(0),
    mGp(0),
    mAttackRange(0),
    mTotalWeight(0), mMaxWeight(0),
    ATK(0), MATK(0), DEF(0), MDEF(0), HIT(0), FLEE(0),
    ATK_BONUS(0), MATK_BONUS(0), DEF_BONUS(0), MDEF_BONUS(0), FLEE_BONUS(0),
    mStatPoint(0), mSkillPoint(0),
    mStatsPointsToAttribute(0),
    mXp(0), mNetwork(0),
    mTarget(NULL), mPickUpTarget(NULL),
    mTrading(false), mGoingToTarget(false),
    mLastAction(-1),
    mWalkingDir(0), mDestX(0), mDestY(0),
    mInventory(new Inventory)
{
}

LocalPlayer::~LocalPlayer()
{
    delete mInventory;
}

void LocalPlayer::logic()
{
    switch (mAction) {
        case WALK:
            mFrame = (get_elapsed_time(mWalkTime) * 6) / mWalkSpeed;
            if (mFrame >= 6) {
                nextStep();
            }
            break;

        case ATTACK:
            int frames = 4;
            if (    mEquippedWeapon
                &&  mEquippedWeapon->getAttackType() == ACTION_ATTACK_BOW)
            {
                frames = 5;
            }
            mFrame = (get_elapsed_time(mWalkTime) * frames) / mAttackSpeed;
            if (mFrame >= frames) {
                nextStep();
                attack();
            }
            break;
    }

    // Actions are allowed once per second
    if (get_elapsed_time(mLastAction) >= 1000) {
        mLastAction = -1;
    }

    Being::logic();
}

void LocalPlayer::nextStep()
{
    if (mPath.empty())
    {
        if (mPickUpTarget)
        {
            pickUp(mPickUpTarget);
        }

        if (mWalkingDir)
        {
            walk(mWalkingDir);
        }

    }

    if (mGoingToTarget && mTarget && withinAttackRange(mTarget))
    {
        mAction = Being::STAND;
        attack(mTarget, true);
        mGoingToTarget = false;
        mPath.clear();
        return;
    }
    else if (mGoingToTarget && !mTarget)
    {
        mGoingToTarget = false;
        mPath.clear();
    }

    Player::nextStep();
}

void LocalPlayer::equipItem(Item *item)
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_PLAYER_EQUIP);
    outMsg.writeInt16(item->getInvIndex());
    outMsg.writeInt16(0);
}

void LocalPlayer::unequipItem(Item *item)
{
    if (!item)
        return;

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_PLAYER_UNEQUIP);
    outMsg.writeInt16(item->getInvIndex());

    // Tidy equipment directly to avoid weapon still shown bug, for instance
    mEquipment->removeEquipment(item->getInvIndex());
}

void LocalPlayer::useItem(Item *item)
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_PLAYER_INVENTORY_USE);
    outMsg.writeInt16(item->getInvIndex());
    outMsg.writeInt32(item->getId());
    // Note: id is dest of item, usually player_node->account_ID ??
}

void LocalPlayer::dropItem(Item *item, int quantity)
{
    // TODO: Fix wrong coordinates of drops, serverside?
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_PLAYER_INVENTORY_DROP);
    outMsg.writeInt16(item->getInvIndex());
    outMsg.writeInt16(quantity);
}

void LocalPlayer::pickUp(FloorItem *item)
{
    int dx = item->getX() - mX;
    int dy = item->getY() - mY;

    if (dx * dx + dy * dy < 4) {
        MessageOut outMsg(mNetwork);
        outMsg.writeInt16(CMSG_ITEM_PICKUP);
        outMsg.writeInt32(item->getId());
        mPickUpTarget = NULL;
    } else {
        setDestination(item->getX(), item->getY());
        mPickUpTarget = item;
        stopAttack();
    }
}

void LocalPlayer::walk(unsigned char dir)
{
    if (!mMap || !dir)
        return;

    if (mAction == WALK && !mPath.empty())
    {
        // Just finish the current action, otherwise we get out of sync
        Being::setDestination(mX, mY);
        return;
    }

    Sint16 dx = 0, dy = 0;
    if (dir & UP)
        dy--;
    if (dir & DOWN)
        dy++;
    if (dir & LEFT)
        dx--;
    if (dir & RIGHT)
        dx++;

    // Prevent skipping corners over colliding tiles
    if (dx && mMap->tileCollides(mX + dx, mY))
        dx = 0;
    if (dy && mMap->tileCollides(mX, mY + dy))
        dy = 0;

    // Choose a straight direction when diagonal target is blocked
    if (dx && dy && mMap->tileCollides(mX + dx, mY + dy))
        dx = 0;

    // Walk to where the player can actually go
    if ((dx || dy) && !mMap->tileCollides(mX + dx, mY + dy))
    {
        setDestination(mX + dx, mY + dy);
    }
    else if (dir)
    {
        // If the being can't move, just change direction
        // TODO: Communicate this to the server (waiting on tmwserv)
        setDirection(dir);
    }
}

void LocalPlayer::setTarget(Being *target)
{
    if (target == mTarget)
    {
        return;
    }
    if (mTarget && mTarget->getType() == Being::MONSTER)
    {
        static_cast<Monster *>(mTarget)->showName(false);
    }
    mTarget = target;
    if (target && target->getType() == Being::MONSTER)
    {
        static_cast<Monster *>(target)->showName(true);
    }
}

void LocalPlayer::setDestination(Uint16 x, Uint16 y)
{
    // Only send a new message to the server when destination changes
    if (x != mDestX || y != mDestY)
    {
        mDestX = x;
        mDestY = y;

        char temp[4] = "";
        MessageOut outMsg(mNetwork);
        set_coordinates(temp, x, y, mDirection);
        outMsg.writeInt16(0x0085);
        outMsg.writeString(temp, 3);
    }

    mPickUpTarget = NULL;
    Being::setDestination(x, y);
}

void LocalPlayer::setWalkingDir(int dir)
{
    mWalkingDir = dir;

    // If we're not already walking, start walking.
    if (mAction != WALK && dir)
    {
        walk(dir);
    }
}

void LocalPlayer::raiseAttribute(Attribute attr)
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_STAT_UPDATE_REQUEST);

    switch (attr)
    {
        case STR:
            outMsg.writeInt16(0x000d);
            break;

        case AGI:
            outMsg.writeInt16(0x000e);
            break;

        case VIT:
            outMsg.writeInt16(0x000f);
            break;

        case INT:
            outMsg.writeInt16(0x0010);
            break;

        case DEX:
            outMsg.writeInt16(0x0011);
            break;

        case LUK:
            outMsg.writeInt16(0x0012);
            break;
    }
    outMsg.writeInt8(1);
}

void LocalPlayer::raiseSkill(Uint16 skillId)
{
    if (mSkillPoint <= 0)
        return;

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_SKILL_LEVELUP_REQUEST);
    outMsg.writeInt16(skillId);
}

void LocalPlayer::toggleSit()
{
    if (mLastAction != -1)
        return;
    mLastAction = tick_time;

    char type;
    switch (mAction)
    {
        case STAND: type = 2; break;
        case SIT: type = 3; break;
        default: return;
    }

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(0x0089);
    outMsg.writeInt32(0);
    outMsg.writeInt8(type);
}

void LocalPlayer::emote(Uint8 emotion)
{
    if (mLastAction != -1)
        return;
    mLastAction = tick_time;

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(0x00bf);
    outMsg.writeInt8(emotion);
}

void LocalPlayer::tradeReply(bool accept)
{
    if (!accept)
        mTrading = false;

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_TRADE_RESPONSE);
    outMsg.writeInt8(accept ? 3 : 4);
}

void LocalPlayer::trade(Being *being) const
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(CMSG_TRADE_REQUEST);
    outMsg.writeInt32(being->getId());
}

bool LocalPlayer::tradeRequestOk() const
{
    return !mTrading;
}

void LocalPlayer::attack(Being *target, bool keep)
{
    // Can only attack when standing still
    if (mAction != STAND)
        return;

    if (keep && target)
    {
        setTarget(target);
    }
    else if (mTarget)
    {
        target = mTarget;
    }

    if (!target)
        return;

    int dist_x = target->mX - mX;
    int dist_y = target->mY - mY;

    if (abs(dist_y) >= abs(dist_x))
    {
        if (dist_y > 0)
            setDirection(DOWN);
        else
            setDirection(UP);
    }
    else
    {
        if (dist_x > 0)
            setDirection(RIGHT);
        else
            setDirection(LEFT);
    }

    setAction(ATTACK);
    mWalkTime = tick_time;

    if (mEquippedWeapon)
    {
        std::string soundFile = mEquippedWeapon->getSound(EQUIP_EVENT_STRIKE);
        if (soundFile != "") sound.playSfx(soundFile);
    }
    else {
        sound.playSfx("sfx/fist-swish.ogg");
    }

    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(0x0089);
    outMsg.writeInt32(target->getId());
    outMsg.writeInt8(0);
}

void LocalPlayer::stopAttack()
{
    setTarget(NULL);
}

Being* LocalPlayer::getTarget() const
{
    return mTarget;
}

void LocalPlayer::revive()
{
    MessageOut outMsg(mNetwork);
    outMsg.writeInt16(0x00b2);
    outMsg.writeInt8(0);
}

void LocalPlayer::setXp(int xp)
{
    if (mMap && xp > mXp)
    {
        const std::string text = toString(xp - mXp) + " xp";

        // Show XP number
        particleEngine->addTextRiseFadeOutEffect(text, hitYellowFont,
                                                 mPx + 16, mPy - 16);
    }
    mXp = xp;
}

bool LocalPlayer::withinAttackRange(Being *target)
{
    int dist_x = abs(target->mX - mX);
    int dist_y = abs(target->mY - mY);

    if (dist_x > getAttackRange() || dist_y > getAttackRange())
    {
        return false;
    }

    return true;
}

void LocalPlayer::setGotoTarget(Being *target)
{
    setTarget(target);
    mGoingToTarget = true;
    setDestination(target->mX, target->mY);
}


extern MiniStatusWindow *miniStatusWindow;

void LocalPlayer::handleStatusEffect(StatusEffect *effect, int effectId)
{
    Being::handleStatusEffect(effect, effectId);



    if (effect) {
        effect->deliverMessage();
        effect->playSFX();

        AnimatedSprite *sprite = effect->getIcon();

        if (!sprite) {
            // delete sprite, if necessary
            for (unsigned int i = 0; i < mStatusEffectIcons.size();)
                if (mStatusEffectIcons[i] == effectId) {
                    mStatusEffectIcons.erase(mStatusEffectIcons.begin() + i);
                    miniStatusWindow->eraseIcon(i);
                } else i++;
        } else {
            // replace sprite or append
            bool found = false;

            for (unsigned int i = 0; i < mStatusEffectIcons.size(); i++)
                if (mStatusEffectIcons[i] == effectId) {
                    miniStatusWindow->setIcon(i, sprite);
                    found = true;
                    break;
                }

            if (!found) { // add new
                int offset = mStatusEffectIcons.size();
                miniStatusWindow->setIcon(offset, sprite);
                mStatusEffectIcons.push_back(effectId);
            }
        }
    }
}
