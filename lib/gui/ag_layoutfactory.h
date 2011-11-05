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

#include <rk_singleton.h>
#include <rk_utf8.h>
#include <ag_widget.h>
#include <ag_xml.h>
#include <ag_layout.h>

class AGEXPORT AGLayoutCreator:public RKSingleton<AGLayoutCreator>
{
 public:
  AGLayoutCreator();//const AGString &pName);
  //  virtual ~AGLayoutCreator();
  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode);
  void setResult(AGWidget *pWidget);
  void setClient(AGWidget *pWidget);
  AGWidget *getResult();
  AGWidget *getClient();

  void clearResult();

 private:

  AGWidget *mWidget,*mClient;


};

class AGEXPORT AGLayoutFactory
{
  std::map<AGString,AGLayoutCreator*> mCreators;

  AGLayoutFactory();
 public:
  ~AGLayoutFactory();

  void addCreator(const AGString &pName,AGLayoutCreator *creator);
  void removeCreator(const AGString &pName,AGLayoutCreator *creator);

  std::pair<AGWidget*,AGWidget*> create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode);

  friend AGLayoutFactory *getLayoutFactory();

  friend void AGLayout::loadXML(const std::string &pXMLData);

  AGLayout *getCurrentLayout();
 private:
  std::list<AGLayout*> mCurrentLayouts;
  void pushLayout(AGLayout *pLayout);
  void popLayout();

};

AGEXPORT AGLayoutFactory *getLayoutFactory();
/*
#define IMPLEMENT_COMPONENT_FACTORY(CLASS) \
AG##CLASS##LayoutCreator factory_##CLASS

#define REGISTER_COMPONENT(CLASS,NAME) AG##CLASS##LayoutCreator():AGLayoutCreator(NAME){}
*/

#define IMPLEMENT_COMPONENT_FACTORY(CLASS)
#define REGISTER_COMPONENT(CLASS,NAME)


#endif
