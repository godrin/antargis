/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_layoutfactory.cc
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

#include "ag_layoutfactory.h"
#include "ag_kill.h"
#include "rk_debug.h"

AGLayoutCreator::AGLayoutCreator() {
}
AGLayoutCreator::~AGLayoutCreator() {
}

/*
 * FIXME
void AGLayoutCreator::setResult(AGWidget *pWidget) {
  mWidget = pWidget;
  if (mWidget)
    getLayoutFactory()->getCurrentLayout()->addChildRef(mWidget);
}
*/


AGLayoutFactory::AGLayoutFactory() { REGISTER_SINGLETON(this); }

AGLayoutFactory::~AGLayoutFactory() {
  for (auto creator : mCreators) {
    delete creator.second;
  }
  mCreators.clear();
}

void AGLayoutFactory::addCreator(const AGString &pName,
                                 AGLayoutCreator *creator) {
  mCreators[pName] = creator;
}

/**
 *  \returns a pair of 2 widgets. The first one is the outer widget, which
 * shoudl be inserted. The second one is the widget, in which all children
 * should be inserted.
 */
std::pair<AGWidget *, AGWidget *> AGLayoutFactory::create(AGWidget *pParent,
                                                          const AGRect2 &pRect,
                                                          const Node &pNode) {
  AGLayoutCreator *creator = mCreators[pNode.getName()];

  if (creator) {
    AGWidget *outer, *inner;
    AGLayoutCreationResult result = creator->create(pParent, pRect, pNode);
    outer = result.getWidget();
    inner = result.getClient();
    if (inner == 0)
      inner = outer;

    return std::make_pair(outer, inner);
  }
  std::string name;
  if (name != "" && name != "colsize" && name != "rowsize") {
    cdebug("no creation at:" << name);
  }
  return std::make_pair((AGWidget *)0, (AGWidget *)0);
}

void AGLayoutFactory::pushLayout(AGLayout *pLayout) {
  mCurrentLayouts.push_back(pLayout);
}
void AGLayoutFactory::popLayout() { mCurrentLayouts.pop_back(); }

AGLayout *AGLayoutFactory::getCurrentLayout() { return mCurrentLayouts.back(); }

AGLayoutFactory *mLayoutFactory = 0;
AGLayoutFactory *getLayoutFactory() {
  if (!mLayoutFactory)
    mLayoutFactory = new AGLayoutFactory;
  return mLayoutFactory;
}
