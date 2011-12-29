/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_layout.cc
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

#include "ag_layout.h"
#include "ag_xml.h"
#include "rk_tools.h"
#include "ag_theme.h"
#include "ag_listbox.h"
#include "ag_combo.h"
#include "ag_image.h"
#include "ag_frame.h"

#include "ag_button.h"
#include "ag_edit.h"
#include "ag_window.h"
#include "ag_text.h"
#include "ag_main.h"
#include "ag_video.h"

#include "ag_layoutfactory.h"

// keep in mind: ag_local.h must be included after ruby.h!!!
#include "ag_local.h"

AGLayout::AGLayout(AGWidget *pgParent):
  AGWidget(pgParent,AGRect2(0,0,0,0))
  {
    CTRACE;
  }

AGLayout::~AGLayout() throw()
  {}


void AGLayout::loadXML(const std::string &pXMLData)
  {
    CTRACE;
    getLayoutFactory()->pushLayout(this);

    // disable GC here
    // marking makes problems here - as the widgets are not inserted into the tree
    // when _() is called, which itself is a ruby-dependent thingy
    Document p;
    p.parseMemory(pXMLData);




    AGWidget *pgParent=getParent();

    AGRect2 geom;
    AGString geomS=p.root().get("geometry");
    if(geomS.length())
      geom=AGRect2(geomS);
    else if(getParent())
      geom=pgParent->getRect().origin();
    else
      geom=AGRect2(0,0,getVideo()->width(),getVideo()->height());


    cdebug("GEOMETRY:"<<geom.toString());

    setRect(geom);
    setName(p.root().get("name"));

    parseChildren(this,p.root());


    if(mTabIndices.size())
      {
        AGWidget *w=mTabIndices.begin()->second;
        w->gainCompleteFocus(); // is ok, because till here layout isn't inserted into screen yet
      }
    getLayoutFactory()->popLayout();
    mTempWidgets.clear();
    initEvents();
  }

bool AGLayout::eventKeyDown(AGEvent *m)
  {
    if(m->getKey()==SDLK_TAB)
      {
        // search element, which has focus
        std::map<int,AGWidget*>::iterator i=mTabIndices.begin();

        for(;i!=mTabIndices.end();i++)
          if(i->second->hasFocus())
            break;

        if(i!=mTabIndices.end())
          i++;
        if(i==mTabIndices.end())
          i=mTabIndices.begin();
        if(i!=mTabIndices.end())
          i->second->gainFocus();

        return true;
      }
    else
      return AGWidget::eventKeyDown(m);
  }


void AGLayout::addTabIndex(int i,AGWidget *pWidget)
  {
    mTabIndices[i]=pWidget;
  }

int AGLayout::getNextTabIndex() const
{
  if(mTabIndices.size()>0)
    return mTabIndices.rbegin()->first+1;
  else
    return 1;
}



AGLayout *getLayout(AGWidget *pWidget)
  {
    AGLayout *l=dynamic_cast<AGLayout*>(pWidget);
    if(l)
      return l;
    else if(pWidget->getParent())
      return getLayout(pWidget->getParent());
    return 0;
  }


AGWidget *parseNode(AGWidget *pParent,const Node &pNode)
  {
    AGString n=pNode.getName();

    AGRect2 geom=getLayoutGeometry(pParent,pNode);

    std::pair<AGWidget*,AGWidget*> result;

    //  cdebug("n:"<<n);

    result=getLayoutFactory()->create(pParent,geom,pNode);
    AGWidget *outer=result.first;
    AGWidget *inner=result.second;

    if(outer!=0 && pNode.get("name").length())
      outer->setName(pNode.get("name"));

    if(outer!=0 && pNode.get("visible")=="false")
      outer->hide();

    if(outer!=0 && pNode.get("tooltip").length())
      outer->setTooltip(_(pNode.get("tooltip")));

    if(outer!=0 && pNode.get("tabindex").length())
      {
        AGLayout *l=getLayout(pParent);
        if(l)
          {
            l->addTabIndex(pNode.get("tabindex").toInt(),outer);
          }
        else
          cdebug("ERROR in parseNode(.):tabindex given but not embedded in layout???");
      }

    if(outer!=0 && pNode.get("cache")=="true")
      outer->setCaching(true);

    parseChildren(inner,pNode);

    if(outer)
      outer->initHandlers();

    return outer;
  }

AGRect2 getLayoutGeometry(AGWidget *pParent,const Node &pNode)
  {
    AGRect2 geom=pParent->getClientRect();
    AGTable *t=dynamic_cast<AGTable*>(pParent);
    if(t)
      if(pNode.get("col")!="" && pNode.get("row")!="")
        {
          int col=pNode.get("col").toInt();
          int row=pNode.get("row").toInt();

          geom=t->getClientRect(col,row);
          if(!pParent->getRect().origin().contains(geom))
            {
              std::cerr<<"parent-geometry doesn't contain geom:"<<std::endl;
              std::cerr<<"parent:"<<pParent->getRect().origin()<<" geom:"<<geom<<std::endl;
              std::cerr<<"node-name:"<<pNode.getName()<<"  parent-name:"<<pParent->getName()<<std::endl;
            }
          assert(pParent->getRect().origin().contains(geom));
        }


    AGString geomS=pNode.get("geometry");
    if(geomS.length())
      geom=AGRect2(geomS);
    return geom;
  }

void parseChildren(AGWidget *pParent,const Node &pNode)
  {
    if(pParent)
      {
        Node::const_iterator i=pNode.begin();
        for(;i!=pNode.end();i++)
          {
            AGWidget *w=parseNode(pParent,**i);
            if(w) {
              cdebug("added some child:"<<w);
              pParent->addChild(w);
            }
            else {
              cdebug("Could create child for node:"<<((*i)->getName()));
            }
          }
      }
  }

void AGLayout::insertTempWidget(AGWidget *pWidget)
  {
    mTempWidgets.insert(pWidget);
  }


