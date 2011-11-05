/*
 * Copyright (c) 2007 by David Kamphausen. All rights reserved.
 *
 * ag_radiogroup.h
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

#ifndef AG_RADIOGROUP_H
#define AG_RADIOGROUP_H

#include <ag_widget.h>

class AGRadio;

// derive from AGWidget, so that it can be put into the hierarchy
class AGEXPORT AGRadioGroup:public AGWidget
{
 public:
  AGRadioGroup(AGWidget *pParent,const AGRect2 &pr);
  virtual ~AGRadioGroup() throw();

  virtual void eventChange(const AGString &p);

  void add(AGRadio *r);
  void erase(AGRadio *r);
  
  AGSignal sigChanged;
 private:
  std::set<AGRadio*> mChildren;
};

#endif
