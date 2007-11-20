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

#include "player.h"

#include "animatedsprite.h"
#include "game.h"
#include "graphics.h"
#include "log.h"

#include "resources/itemdb.h"
#include "resources/iteminfo.h"

#include "utils/tostring.h"

#include "gui/gui.h"

Player::Player(int id, int job, Map *map):
    Being(id, job, map)
{
}

void
Player::logic()
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
            }
            break;
    }

    Being::logic();
}

Being::Type
Player::getType() const
{
    return PLAYER;
}

void
Player::drawName(Graphics *graphics, int offsetX, int offsetY)
{
    int px = mPx + offsetX;
    int py = mPy + offsetY;

    graphics->setFont(speechFont);
    graphics->setColor(gcn::Color(255, 255, 255));
    graphics->drawText(mName, px + 15, py + 30, gcn::Graphics::CENTER);
}

void Player::setGender(int gender)
{
    // Players can only be male or female
    if (gender > 1)
    {
        logger->log("Warning: unsupported gender %i, assuming male.", gender);
        gender = 0;
    }

    if (gender != mGender)
    {
        // Reload base sprite
        AnimatedSprite *newBaseSprite;
        if (gender == 0)
        {
            newBaseSprite = AnimatedSprite::load(
                    "graphics/sprites/player_male_base.xml");
        }
        else
        {
            newBaseSprite = AnimatedSprite::load(
                    "graphics/sprites/player_female_base.xml");
        }

        delete mSprites[BASE_SPRITE];
        mSprites[BASE_SPRITE] = newBaseSprite;

        // Reload equipment
        for (int i = 1; i < VECTOREND_SPRITE; i++)
        {
            if (i != HAIR_SPRITE && mEquipmentSpriteIDs.at(i) != 0)
            {
                AnimatedSprite *newEqSprite = AnimatedSprite::load(
                        "graphics/sprites/" + ItemDB::get(
                            mEquipmentSpriteIDs.at(i)).getSprite(gender));
                delete mSprites[i];
                mSprites[i] = newEqSprite;
            }
        }
    }

    Being::setGender(gender);
}

void Player::setHairColor(int color)
{
    if (color != mHairColor && mHairStyle > 0)
    {
        const std::string hairStyle = toString(getHairStyle());
        const std::string gender = (mGender == 0) ? "-male" : "-female";

        AnimatedSprite *newHairSprite = AnimatedSprite::load(
                "graphics/sprites/hairstyle" + hairStyle + gender + ".xml",
                color - 1);
        if (newHairSprite)
            newHairSprite->setDirection(getSpriteDirection());

        delete mSprites[HAIR_SPRITE];
        mSprites[HAIR_SPRITE] = newHairSprite;

        setAction(mAction);
    }

    Being::setHairColor(color);
}

void Player::setHairStyle(int style)
{
    if (style != mHairStyle && mHairColor > 0)
    {
        const std::string hairStyle = toString(style);
        const std::string gender = (mGender == 0) ? "-male" : "-female";

        AnimatedSprite *newHairSprite = AnimatedSprite::load(
                "graphics/sprites/hairstyle" + hairStyle + gender + ".xml",
                mHairColor - 1);
        if (newHairSprite)
            newHairSprite->setDirection(getSpriteDirection());

        delete mSprites[HAIR_SPRITE];
        mSprites[HAIR_SPRITE] = newHairSprite;

        setAction(mAction);
    }

    Being::setHairStyle(style);
}

void Player::setVisibleEquipment(int slot, int id)
{
    // id = 0 means unequip
    if (id == 0)
    {
        delete mSprites[slot];
        mSprites[slot] = NULL;
    }
    else
    {
        AnimatedSprite *equipmentSprite = AnimatedSprite::load(
            "graphics/sprites/" + ItemDB::get(id).getSprite(mGender));

        if (equipmentSprite)
            equipmentSprite->setDirection(getSpriteDirection());

        delete mSprites[slot];
        mSprites[slot] = equipmentSprite;

        if (slot == WEAPON_SPRITE)
        {
            mEquippedWeapon = &ItemDB::get(id);
        }

        setAction(mAction);
    }

    Being::setVisibleEquipment(slot, id);
}
