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
#include "being.h"

#include <algorithm>
#include <cmath>

#include "animatedsprite.h"
#include "equipment.h"
#include "game.h"
#include "graphics.h"
#include "log.h"
#include "map.h"

#include "resources/spriteset.h"

#include "gui/gui.h"

#include "utils/dtor.h"
#include "utils/tostring.h"

extern Spriteset *emotionset;

PATH_NODE::PATH_NODE(Uint16 iX, Uint16 iY):
    x(iX), y(iY)
{
}

Being::Being(Uint16 id, Uint16 job, Map *map):
    mJob(job),
    mX(0), mY(0), mDirection(DOWN),
    mAction(STAND),
    mWalkTime(0),
    mEmotion(0), mEmotionTime(0),
    mAttackSpeed(350),
    mEquipment(new Equipment()),
    mId(id),
    mSex(2),
    mWeapon(0),
    mWalkSpeed(150),
    mMap(NULL),
    mHairStyle(0), mHairColor(0),
    mSpeechTime(0),
    mDamageTime(0),
    mShowSpeech(false), mShowDamage(false)
{
    setMap(map);
    mSprites.resize(VECTOREND_SPRITE, NULL);
}

Being::~Being()
{
    std::for_each(mSprites.begin(), mSprites.end(), make_dtor(mSprites));
    clearPath();
    setMap(NULL);
}

void
Being::setDestination(Uint16 destX, Uint16 destY)
{
    if (!mMap || (mX == destX && mY == destY))
    {
        return;
    }

    Path p;
    if (mX / 32 != destX / 32 || mY / 32 != destY / 32)
    {
        p = mMap->findPath(mX / 32, mY / 32, destX / 32, destY / 32);
        if (p.empty())
        {
            setPath(p);
            return;
        }
        // Remove last tile so that it can be replaced by the exact destination.
        p.pop_back();
        for (Path::iterator i = p.begin(), i_end = p.end(); i != i_end; ++i)
        {
            // Set intermediate step to tile centers.
            i->x = i->x * 32 + 16;
            i->y = i->y * 32 + 16;
        }
    }
    p.push_back(PATH_NODE(destX, destY));
    setPath(p);
}

void
Being::clearPath()
{
    mPath.clear();
}

void
Being::setPath(const Path &path)
{
    mPath = path;

    if (mAction != WALK && mAction != DEAD)
    {
        mWalkTime = tick_time;
        mStepTime = 0;
        nextStep();
    }
}

void
Being::setHairColor(Uint16 color)
{
    mHairColor = (color < NR_HAIR_COLORS) ? color : 0;
}

void
Being::setHairStyle(Uint16 style)
{
    mHairStyle = (style < NR_HAIR_STYLES) ? style : 0;
}

void
Being::setVisibleEquipment(Uint8 slot, Uint8 id)
{
}

void
Being::setSpeech(const std::string &text, Uint32 time)
{
    mSpeech = text;
    mSpeechTime = tick_time;
    mShowSpeech = true;
}

void
Being::setDamage(Sint16 amount, Uint32 time)
{
    mDamage = amount ? toString(amount) : "miss";
    mDamageTime = tick_time;
    mShowDamage = true;
}

void
Being::setMap(Map *map)
{
    // Remove sprite from potential previous map
    if (mMap != NULL)
    {
        mMap->removeSprite(mSpriteIterator);
    }

    mMap = map;

    // Add sprite to potential new map
    if (mMap != NULL)
    {
        mSpriteIterator = mMap->addSprite(this);
    }
}

void
Being::setAction(Uint8 action)
{
    SpriteAction currentAction = ACTION_STAND;
    switch (action)
    {
        case WALK:
            currentAction = ACTION_WALK;
            break;
        case SIT:
            currentAction = ACTION_SIT;
            break;
        case ATTACK:
            if (getType() == MONSTER)
            {
                currentAction = ACTION_DEAD;
            }
            else {
                switch (getWeapon())
                {
                    case 2:
                        currentAction = ACTION_ATTACK_BOW;
                        break;
                    case 1:
                        currentAction = ACTION_ATTACK_STAB;
                        break;
                    case 0:
                        currentAction = ACTION_ATTACK;
                        break;
                }
            };
            break;
        case MONSTER_ATTACK:
            currentAction = ACTION_ATTACK;
            break;
        case DEAD:
            currentAction = ACTION_DEAD;
            break;
        default:
            currentAction = ACTION_STAND;
            break;
    }

    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (!mSprites[i])
            continue;

        if (currentAction == ACTION_ATTACK ||
                currentAction == ACTION_ATTACK_STAB ||
                currentAction == ACTION_ATTACK_BOW)
        {
            mSprites[i]->play(currentAction, mAttackSpeed);
        }
        else
        {
            mSprites[i]->play(currentAction);
        }
    }

    mAction = action;
}

void
Being::setDirection(Uint8 direction)
{
    mDirection = direction;
    SpriteDirection dir = getSpriteDirection();

    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
            mSprites[i]->setDirection(dir);
    }
}

SpriteDirection
Being::getSpriteDirection() const
{
    SpriteDirection dir;

    if (mDirection & UP)
    {
        dir = DIRECTION_UP;
    }
    else if (mDirection & RIGHT)
    {
        dir = DIRECTION_RIGHT;
    }
    else if (mDirection & DOWN)
    {
        dir = DIRECTION_DOWN;
    }
    else {
        dir = DIRECTION_LEFT;
    }

    return dir;
}

void
Being::nextStep()
{
    if (mPath.empty())
    {
        setAction(STAND);
        return;
    }

    PATH_NODE node = mPath.front();
    mPath.pop_front();

    int dir = 0;
    if (node.x > mX)
        dir |= RIGHT;
    else if (node.x < mX)
        dir |= LEFT;
    if (node.y > mY)
        dir |= DOWN;
    else if (node.y < mY)
        dir |= UP;

    setDirection(dir);

    mStepX = node.x - mX;
    mStepY = node.y - mY;
    mX = node.x;
    mY = node.y;
    setAction(WALK);
    mWalkTime += mStepTime / 10;
    mStepTime = mWalkSpeed * (int)std::sqrt((double)mStepX * mStepX + (double)mStepY * mStepY) / 32;
}

void
Being::logic()
{
    // Determine whether the being should take another step
    if (mAction == WALK && get_elapsed_time(mWalkTime) >= mStepTime)
    {
        nextStep();
    }

    // Determine whether speech should still be displayed
    if (get_elapsed_time(mSpeechTime) > 5000)
    {
        mShowSpeech = false;
    }

    // Determine whether damage should still be displayed
    if (get_elapsed_time(mDamageTime) > 3000)
    {
        mShowDamage = false;
    }

    // Update pixel coordinates
    mPx = mX - 16 + getXOffset();
    mPy = mY - 16 + getYOffset();

    if (mEmotion != 0)
    {
        mEmotionTime--;
        if (mEmotionTime == 0) {
            mEmotion = 0;
        }
    }

    // Update sprite animations
    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
        {
            mSprites[i]->update(tick_time * 10);
        }
    }
}

void
Being::draw(Graphics *graphics, int offsetX, int offsetY)
{
    int px = mPx + offsetX;
    int py = mPy + offsetY;

    for (int i = 0; i < VECTOREND_SPRITE; i++)
    {
        if (mSprites[i] != NULL)
        {
            mSprites[i]->draw(graphics, px, py);
        }
    }
}

void
Being::drawEmotion(Graphics *graphics, Sint32 offsetX, Sint32 offsetY)
{
    if (!mEmotion)
        return;

    int px = mPx + offsetX + 3;
    int py = mPy + offsetY - 60;

    graphics->drawImage(emotionset->get(mEmotion - 1), px, py);
}

void
Being::drawSpeech(Graphics *graphics, Sint32 offsetX, Sint32 offsetY)
{
    int px = mPx + offsetX;
    int py = mPy + offsetY;

    // Draw speech above this being
    if (mShowSpeech)
    {
        graphics->setFont(speechFont);
        graphics->setColor(gcn::Color(255, 255, 255));
        graphics->drawText(mSpeech, px + 18, py - 60, gcn::Graphics::CENTER);
    }

    // Draw damage above this being
    if (mShowDamage)
    {
        // Selecting the right color
        if (mDamage == "miss")
        {
            graphics->setFont(hitYellowFont);
        }
        else if (getType() == MONSTER)
        {
            graphics->setFont(hitBlueFont);
        }
        else
        {
            graphics->setFont(hitRedFont);
        }

        int textY = (getType() == MONSTER) ? 32 : 70;
        int ft = get_elapsed_time(mDamageTime) - 1500;
        float a = (ft > 0) ? 1.0 - ft / 1500.0 : 1.0;

        graphics->setColor(gcn::Color(255, 255, 255, (int)(255 * a)));
        graphics->drawText(mDamage,
                           px + 16,
                           py - textY - get_elapsed_time(mDamageTime) / 100,
                           gcn::Graphics::CENTER);

        // Reset alpha value
        graphics->setColor(gcn::Color(255, 255, 255));
    }
}

Being::Type
Being::getType() const
{
    return UNKNOWN;
}

void
Being::setWeaponById(Uint16 weapon)
{
    switch (weapon)
    {
    case 529: // iron arrows
    case 1199: // arrows
        break;

    case 1200: // bow
    case 530: // short bow
    case 545: // forest bow
        setWeapon(2);
        break;

    case 521: // sharp knife
    case 522: // dagger
    case 536: // short sword
    case 1201: // knife
        setWeapon(1);
        break;

    case 0: // unequip
        setWeapon(0);
        break;

    default:
        logger->log("Not a weapon: %d", weapon);
    }
}

int Being::getOffset(int step) const
{
    // Check whether we're walking in the requested direction
    if (mAction != WALK || step == 0) {
        return 0;
    }

    int offset = (get_elapsed_time(mWalkTime) * std::abs(step)) / mStepTime;

    // We calculate the offset _from_ the _target_ location
    offset -= std::abs(step);
    if (offset > 0) {
        offset = 0;
    }

    // Going into negative direction? Invert the offset.
    if (step < 0) {
        offset = -offset;
    }

    return offset;
}
