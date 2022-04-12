/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_screen.cc
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

#include "ag_screen.h"
#include "ag_fontengine.h"
#include "ag_surface.h"
#include "ag_texture.h"
#include "rk_debug.h"

////////////////////////////////////////////////////////////////////////
// AGScreen
////////////////////////////////////////////////////////////////////////

AGScreen *mAGGScreen = 0;
AGScreen *AGEXPORT getScreenPtr() { return mAGGScreen; }

AGScreen &AGEXPORT getScreen() {
  assert(mAGGScreen);
  return *mAGGScreen;
}

void setScreen(AGScreen *s) { mAGGScreen = s; }
AGScreen::AGScreen() { mFontEngine = new AGFontEngine(); }
AGScreen::~AGScreen() throw() { delete mFontEngine; }

void AGScreen::begin() {}

void AGScreen::flip() {}

void AGScreen::update(const std::list<AGRect2> &rs) {}

size_t AGScreen::getWidth() const { return 0; }
size_t AGScreen::getHeight() const { return 0; }

AGSurface AGScreen::screenshotSurface(bool frontBuffer) {
  cdebug("NOT YET IMPLEMENTED");
  return AGSurface(0, 0);
}
AGTexture AGScreen::screenshot(bool frontBuffer) {
  cdebug("NOT YET IMPLEMENTED");
  return AGTexture(0, 0);
}

AGFontEngine *AGScreen::getFontEngine() { return mFontEngine; }
