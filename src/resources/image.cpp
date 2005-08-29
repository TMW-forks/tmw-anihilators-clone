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

#include "image.h"

#include <SDL_image.h>

#include "../log.h"

#ifdef USE_OPENGL
bool Image::useOpenGL = false;
#endif

Image::Image(const std::string &idPath, SDL_Surface *image):
    Resource(idPath), image(image)
{
    // Default to opaque
    alpha = 1.0f;

    bounds.x = 0;
    bounds.y = 0;
    bounds.w = image->w;
    bounds.h = image->h;
}

#ifdef USE_OPENGL
Image::Image(const std::string &idPath, GLuint glimage, int width, int height,
        int texWidth, int texHeight):
    Resource(idPath),
    glimage(glimage),
    texWidth(texWidth),
    texHeight(texHeight)
{
    // Default to opaque
    alpha = 1.0f;

    bounds.x = 0;
    bounds.y = 0;
    bounds.w = width;
    bounds.h = height;
}
#endif

Image::~Image()
{
    unload();
}

Image* Image::load(void* buffer, unsigned int bufferSize, const std::string &idPath)
{
    // Load the raw file data from the buffer in an RWops structure
    SDL_RWops *rw = SDL_RWFromMem(buffer, bufferSize);

    // Use SDL_Image to load the raw image data and have it free the data
    SDL_Surface *tmpImage = IMG_Load_RW(rw, 1);

    if (tmpImage == NULL) {
        logger->log("Error, image load failed");
        return NULL;
    }

    // Determine 32-bit masks based on byte order
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    // Convert the image to a 32 bit software surface for processing
    SDL_Surface *formatImage = SDL_CreateRGBSurface(SDL_SWSURFACE, 0, 0, 32,
            rmask, gmask, bmask, amask);

    if (formatImage == NULL) {
        logger->log("Error, image load failed: not enough memory");
        SDL_FreeSurface(tmpImage);
        return NULL;
    }

    SDL_Surface *image = SDL_ConvertSurface(
            tmpImage, formatImage->format, SDL_SWSURFACE);

    SDL_FreeSurface(formatImage);

    if (image == NULL) {
        logger->log("Error, image load failed: not enough memory");
        return NULL;
    }

    bool hasPink = false;
    bool hasAlpha = false;
    int i;
    Uint32 pink = SDL_MapRGB(image->format, 255, 0, 255);

    // Figure out whether the image has pink pixels
    for (i = 0; i < image->w * image->h; ++i)
    {
        if (((Uint32*)image->pixels)[i] == pink)
        {
            hasPink = true;
            break;
        }
    }

    // Figure out whether the image uses its alpha layer
    for (i = 0; i < image->w * image->h; ++i)
    {
        Uint8 r, g, b, a;
        SDL_GetRGBA(
                ((Uint32*)image->pixels)[i],
                image->format,
                &r, &g, &b, &a);

        if (a != 255)
        {
            hasAlpha = true;
            break;
        }
    }

    SDL_FreeSurface(image);

    if (hasPink && !hasAlpha) {
        SDL_SetColorKey(tmpImage, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                SDL_MapRGB(tmpImage->format, 255, 0, 255));
    } else if (hasAlpha) {
        SDL_SetAlpha(tmpImage, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_OPAQUE);
    }

#ifdef USE_OPENGL
    if (useOpenGL)
    {
        int width = tmpImage->w;
        int height = tmpImage->h;
        int realWidth = 1, realHeight = 1;

        while (realWidth < width && realWidth < 1024) {
            realWidth *= 2;
        }

        while (realHeight < height && realHeight < 1024) {
            realHeight *= 2;
        }

        SDL_SetAlpha(tmpImage, 0, SDL_ALPHA_OPAQUE);
        SDL_Surface *oldImage = tmpImage;
        tmpImage = SDL_CreateRGBSurface(SDL_SWSURFACE, realWidth, realHeight, 32,
                rmask, gmask, bmask, amask);

        if (tmpImage == NULL) {
            logger->log("Error, image convert failed: out of memory");
            return NULL;
        }

        SDL_BlitSurface(oldImage, NULL, tmpImage, NULL);
        SDL_FreeSurface(oldImage);

        GLuint texture;
        glGenTextures(1, &texture);
        logger->log("Binding texture %d (%dx%d)",
                texture, tmpImage->w, tmpImage->h);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (SDL_MUSTLOCK(tmpImage)) {
            SDL_LockSurface(tmpImage);
        }

        glTexImage2D(
                GL_TEXTURE_2D, 0, 4,
                tmpImage->w, tmpImage->h,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                tmpImage->pixels);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (SDL_MUSTLOCK(tmpImage)) {
            SDL_UnlockSurface(tmpImage);
        }

        SDL_FreeSurface(tmpImage);

        GLenum error = glGetError();
        if (error)
        {
            std::string errmsg = "Unkown error";
            switch (error)
            {
                case GL_INVALID_ENUM:
                    errmsg = "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    errmsg = "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    errmsg = "GL_INVALID_OPERATION";
                    break;
                case GL_STACK_OVERFLOW:
                    errmsg = "GL_STACK_OVERFLOW";
                    break;
                case GL_STACK_UNDERFLOW:
                    errmsg = "GL_STACK_UNDERFLOW";
                    break;
                case GL_OUT_OF_MEMORY:
                    errmsg = "GL_OUT_OF_MEMORY";
                    break;
            }
            logger->log("Error: Image GL import failed: %s", errmsg.c_str());
            return NULL;
        }

        return new Image(idPath, texture, width, height, realWidth, realHeight);
    }
#endif

    // Set color key and alpha blending optins, and convert the surface to the
    // current display format
    SDL_Surface *prevImage = tmpImage;
    if (hasAlpha) {
        image = SDL_DisplayFormatAlpha(tmpImage);
    }
    else {
        image = SDL_DisplayFormat(tmpImage);
    }
    SDL_FreeSurface(prevImage);

    if (image == NULL) {
        logger->log("Error: Image convert failed.");
        return NULL;
    }

    return new Image(idPath, image);
}

void Image::unload()
{
    loaded = false;

#ifdef USE_OPENGL
    if (useOpenGL) {
        return;
    }
#endif

    if (!image) {
        return;
    }

    // Free the image surface.
    SDL_FreeSurface(image);
    image = NULL;
}

int Image::getWidth() const
{
    return bounds.w;
}

int Image::getHeight() const
{
    return bounds.h;
}

Image *Image::getSubImage(int x, int y, int width, int height)
{
    // Create a new clipped sub-image
#ifdef USE_OPENGL
    if (useOpenGL) {
        return new SubImage(this, glimage, x, y, width, height, texWidth, texHeight);
    }
#endif

    return new SubImage(this, image, x, y, width, height);
}

void Image::setAlpha(float a)
{
    alpha = a;

#ifdef USE_OPENGL
    if (useOpenGL) {
        return;
    }
#endif

    // Set the alpha value this image is drawn at
    SDL_SetAlpha(image, SDL_SRCALPHA | SDL_RLEACCEL, (int)(255 * alpha));
}

float Image::getAlpha()
{
    return alpha;
}

#ifdef USE_OPENGL
void Image::setLoadAsOpenGL(bool useOpenGL)
{
    Image::useOpenGL = useOpenGL;
}
#endif

//============================================================================
// SubImage Class
//============================================================================

SubImage::SubImage(Image *parent, SDL_Surface *image,
        int x, int y, int width, int height):
    Image("", image), parent(parent)
{
    parent->incRef();

    // Set up the rectangle.
    bounds.x = x;
    bounds.y = y;
    bounds.w = width;
    bounds.h = height;
}

#ifdef USE_OPENGL
SubImage::SubImage(Image *parent, GLuint image,
        int x, int y, int width, int height, int texWidth, int texHeight):
    Image("", image, width, height, texWidth, texHeight), parent(parent)
{
    parent->incRef();

    // Set up the rectangle.
    bounds.x = x;
    bounds.y = y;
    bounds.w = width;
    bounds.h = height;
}
#endif

SubImage::~SubImage()
{
#ifdef USE_OPENGL
    if (!useOpenGL) {
        image = NULL;
    }
#else
    image = NULL;
#endif

    parent->decRef();
}

Image *SubImage::getSubImage(int x, int y, int w, int h)
{
    return NULL;
}
