/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_layoutfactory.h
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

#ifndef AG_LAYOUTFACTORY_H
#define AG_LAYOUTFACTORY_H

#include <ag_layout.h>
#include <ag_widget.h>
#include <ag_xml.h>
#include <rk_utf8.h>

class AGEXPORT AGLayoutCreationResult {
  public:
  AGLayoutCreationResult(AGWidget *pWidget) {
    mWidget = pWidget;
    mClient = 0;
  }
  AGLayoutCreationResult(AGWidget *pWidget, AGWidget *pClient) :mWidget(pWidget), mClient(pClient) {
    }
  AGWidget*getWidget() {
    return mWidget;
  }
  AGWidget*getClient() {
    if(mClient) {
      return mClient;
    }
    return mWidget;
  }

  private:
  AGWidget *mWidget;
  AGWidget *mClient;
};

class AGEXPORT AGLayoutCreator {
public:
  AGLayoutCreator();
  virtual ~AGLayoutCreator();
  virtual AGLayoutCreationResult create(AGWidget *pParent, const AGRect2 &pRect,
      const Node &pNode)=0;
};

class AGEXPORT AGLayoutFactory {
  std::map<AGString, AGLayoutCreator *> mCreators;

  AGLayoutFactory();

public:
  ~AGLayoutFactory();

  // creators are claimed and will be deleted on destruction of AGLayoutFactory
  void addCreator(const AGString &pName, AGLayoutCreator *creator);

  std::pair<AGWidget *, AGWidget *>
  create(AGWidget *pParent, const AGRect2 &pRect, const Node &pNode);

  friend AGLayoutFactory *getLayoutFactory();

  friend void AGLayout::loadXML(const std::string &pXMLData);

  AGLayout *getCurrentLayout();

private:
  std::list<AGLayout *> mCurrentLayouts;
  void pushLayout(AGLayout *pLayout);
  void popLayout();
};

AGEXPORT AGLayoutFactory *getLayoutFactory();

#endif
