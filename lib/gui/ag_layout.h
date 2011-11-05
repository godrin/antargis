/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_layout.h
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

#ifndef AG_LAYOUT_H
#define AG_LAYOUT_H

#include "ag_widget.h"
#include "ag_xml.h"

class AGTable;

class AGEXPORT AGLayout:public AGWidget
{
 public:
  AGLayout(AGWidget *pgParent);
  virtual ~AGLayout() throw();
  
  virtual void loadXML(const std::string &pXMLData);

  void addTabIndex(int i,AGWidget *pWidget);

  virtual bool eventKeyDown(AGEvent *m);
  int getNextTabIndex() const;

  static void registerLayouts();

 private:
  void insertTempWidget(AGWidget *pWidget);
   
  std::map<int,AGWidget*> mTabIndices;
  
  std::set<AGWidget*> mTempWidgets;
};

AGEXPORT AGWidget *parseNode(AGWidget *pParent,const Node &pNode);
AGEXPORT void parseChildren(AGWidget *pParent,const Node &pNode);
//AGTable *parseTable(AGWidget *pParent,const Node &pNode,const AGRect2 &geom);
AGEXPORT AGRect2 getLayoutGeometry(AGWidget *pParent,const Node &pNode);

#endif
