/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * mpng.h
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef MPNG_H
#define MPNG_H

#include <string>

#include <SDL.h>

AGEXPORT std::string toPNG(const SDL_Surface *s);
AGEXPORT SDL_Surface *fromPNG(std::string s);


#endif
