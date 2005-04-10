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

#ifndef _TMW_SOUND_EFFECT_H
#define _TMW_SOUND_EFFECT_H

#include "resource.h"
#include <SDL.h>
#include <SDL_mixer.h>

/**
 * Defines a class for loading and storing sound effects.
 */
class SoundEffect : public Resource
{
    public:
        /**
         * Destructor.
         */
        virtual ~SoundEffect();

        /**
         * Loads a sample from a buffer in memory.
         *
         * @param buffer     The memory buffer containing the sample data.
         * @param bufferSize The size of the memory buffer in bytes.
         *
         * @return <code>NULL</code> if the an error occurred, a valid pointer
         *         otherwise.
         */
        static SoundEffect *load(void* buffer, unsigned int bufferSize);

        /**
         * Frees the resources created by SDL.
         */
        virtual void unload();

        /**
         * Plays the sample.
         *
         * @param loops     Number of times to repeat the playback.
         * @param volume    Sample playback volume.
         *
         * @return <code>true</code> if the playback started properly
         *         <code>false</code> otherwise.
         */
        virtual bool play(int loops, int volume);

    protected:
        /**
         * Constructor.
         */
        SoundEffect(Mix_Chunk *soundEffect);

        Mix_Chunk *soundEffect;
};

#endif
