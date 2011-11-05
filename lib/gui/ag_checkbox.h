/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_checkbox.h
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

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#ifndef AG_CHECK_BOX_H
#define AG_CHECK_BOX_H

//#include "ag_vtiler.h"
#include "ag_button.h"

class AGImage;

class AGEXPORT AGCheckBox:public AGButton
{
 public:
  AGCheckBox(AGWidget *pParent,AGRect2 pRect);

  void setSurfaces(AGSurface pDisabledSurface,AGSurface pEnabledSurface);

  void setState(const State &pState);
  virtual bool eventMouseClick(AGEvent *m);
 private:
  AGSurface *mSurfaces[2];
};

#endif
