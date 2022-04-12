/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_checkbox.cc
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

#include "ag_checkbox.h"
#include "ag_image.h"
#include "ag_layoutfactory.h"
#include "ag_local.h"
#include "ag_text.h"
#include "ag_theme.h"
#include "rk_debug.h"

AGCheckBox::AGCheckBox(AGWidget *pParent, AGRect2 pRect)
    : AGButton(pParent, pRect, "") {
  mSurfaces[0] = mSurfaces[1] = 0;
}

bool AGCheckBox::eventMouseClick(AGEvent *m) {
  CTRACE;
  setChecked(!isChecked());
  return AGButton::eventMouseClick(m); // false;//true; // eat
}

void AGCheckBox::setSurfaces(AGSurface pDisabledSurface,
                             AGSurface pEnabledSurface) {
  checkedDelete(mSurfaces[0]);
  checkedDelete(mSurfaces[1]);
  mSurfaces[0] = new AGSurface(pDisabledSurface);
  mSurfaces[1] = new AGSurface(pEnabledSurface);

  //  setState(getState());
  queryRedraw();
}

void AGCheckBox::setState(const State &pState) {
  if (mSurfaces[0]) {
    if (pState == NORMAL || pState == LIGHTED || pState == PRESSED)
      setSurface(*(mSurfaces[0]));
    else
      setSurface(*(mSurfaces[1]));
  }

  AGButton::setState(pState);
}
