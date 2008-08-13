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
 *  $Id: monster.cpp 3823 2007-12-28 18:36:58Z crush_tmw $
 */

#include "monster.h"

#include "animatedsprite.h"
#include "game.h"
#include "sound.h"
#include "particle.h"
#include "text.h"
#include "localplayer.h"

#include "gui/gui.h"

#include "resources/monsterdb.h"

#include "utils/tostring.h"

static const int NAME_X_OFFSET = 16;
static const int NAME_Y_OFFSET = 16;

Monster::Monster(Uint32 id, Uint16 job, Map *map):
    Being(id, job, map),
    mText(0)
{
    const MonsterInfo&  info = MonsterDB::get(job - 1002);

    std::string filename = info.getSprite();
    if (filename != "")
    {
        mSprites[BASE_SPRITE] = AnimatedSprite::load(
            "graphics/sprites/" + filename);
    }
    else
    {
        mSprites[BASE_SPRITE] = AnimatedSprite::load("graphics/sprites/error.xml");
    }

    const std::list<std::string> &particleEffects = info.getParticleEffects();
    for (   std::list<std::string>::const_iterator i = particleEffects.begin();
            i != particleEffects.end();
            i++
        )
    {
        controlParticle(particleEngine->addEffect((*i), 0, 0));
    }
}

Monster::~Monster()
{
    if (mText)
    {
        player_node->setTarget(0);
    }
}

void
Monster::logic()
{
    if (mAction != STAND)
    {
        mFrame = (get_elapsed_time(mWalkTime) * 4) / mWalkSpeed;

        if (mFrame >= 4 && mAction != DEAD)
        {
            nextStep();
        }
    }

    Being::logic();
}

Being::Type
Monster::getType() const
{
    return MONSTER;
}

void
Monster::setAction(Uint8 action)
{
    SpriteAction currentAction = ACTION_INVALID;

    switch (action)
    {
        case WALK:
            currentAction = ACTION_WALK;
            break;
        case DEAD:
            currentAction = ACTION_DEAD;
            sound.playSfx(getInfo().getSound(MONSTER_EVENT_DIE));
            break;
        case ATTACK:
            currentAction = ACTION_ATTACK;
            mSprites[BASE_SPRITE]->reset();
            break;
        case STAND:
            currentAction = ACTION_STAND;
            break;
        case HURT:
            // Not implemented yet
            break;
    }

    if (currentAction != ACTION_INVALID)
    {
        mSprites[BASE_SPRITE]->play(currentAction);
        mAction = action;
    }
}

void
Monster::handleAttack(Being *victim, int damage)
{
    Being::handleAttack(victim, damage);

    const MonsterInfo &mi = getInfo();
    sound.playSfx(mi.getSound((damage > 0) ? MONSTER_EVENT_HIT : MONSTER_EVENT_MISS));
}

void
Monster::takeDamage(int amount)
{
    if (amount > 0) sound.playSfx(getInfo().getSound(MONSTER_EVENT_HURT));
    Being::takeDamage(amount);
}

Being::TargetCursorSize
Monster::getTargetCursorSize() const
{
    return getInfo().getTargetCursorSize();
}

const MonsterInfo&
Monster::getInfo() const
{
    return MonsterDB::get(mJob - 1002);
}

void Monster::showName(bool show)
{
    if (mText)
    {
        delete mText;
    }
    if (show)
    {
        mText = new Text(getInfo().getName(), mPx + NAME_X_OFFSET,
                         mPy + NAME_Y_OFFSET - getHeight(),
                         gcn::Graphics::CENTER,
                         mobNameFont, gcn::Color(255, 32, 32));
    }
    else
    {
        mText = 0;
    }
}

void Monster::updateCoords()
{
    if (mText)
    {
        mText->adviseXY(mPx + NAME_X_OFFSET,
                        mPy + NAME_Y_OFFSET - getHeight());
    }
}
