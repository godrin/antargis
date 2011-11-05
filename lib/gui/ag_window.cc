/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_window.cc
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

#include "ag_window.h"
#include "ag_surface.h"
#include "ag_theme.h"
#include "ag_image.h"
#include "ag_text.h"
#include "rk_debug.h"
#include "ag_button.h"
#include "ag_caption.h"
#include "ag_texturecache.h"

#undef connect

AGWindow::AGWindow(AGWidget *pWidget,const AGRect2 &pRect,const AGStringUtf8 &pTitle,const AGString &pTheme):
  AGTable(pWidget,pRect),sigClose(this,"sigClose"),mTitle(pTitle)

  {
    CTRACE;

    AGLocalTheme theme=getTheme()->getTheme(pTheme);

    AGString borderImage="window.border.image";

    AGSurface s=theme.getSurface(borderImage);
    float bw=s.getRect().w()/3;
    int titBarHeight=20;

    AGTable *t=0;


    std::vector<const AGTexture*> textures;

    // build textures
    for(int y=0;y<3;y++)
      for(int x=0;x<3;x++)
        {
          AGRect2 r(x*bw,y*bw,bw,bw);

          textures.push_back(&getTextureCache()->get(theme.getSurfaceName(borderImage),r));
        }




    if(pTitle.length())
      {
        // use 4 rows
        addFixedRow(bw); // upper border
        addFixedRow(titBarHeight); // titletext and button
        //      addFixedRow(bw); // lower border below titletext
        addRow(1.0);
        addFixedRow(bw); // lower lower below window
        addFixedColumn(bw);
        addColumn(1.0);
        addFixedColumn(bw);

        AGTable::addChild(0,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[0],true));
        AGTable::addChild(1,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[1],true));
        AGTable::addChild(2,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[2],true));

        AGImage *i1,*i2;

        AGTable::addChild(0,1,i1=new AGImage(this,AGRect2(0,0,bw,titBarHeight),*textures[3],true));
        // title
        t=dynamic_cast<AGTable*>(getTitleBar((int)(width()-2*bw),titBarHeight));

        AGTable::addChild(2,1,i2=new AGImage(this,AGRect2(0,0,bw,titBarHeight),*textures[5],true));
        i1->setHeight(t->height());
        i2->setHeight(t->height());

        cdebug("i2:"<<i2->width()<<"   "<<i2->height());
        cdebug("bw:"<<bw);


        AGTable::addChild(0,2,new AGImage(this,AGRect2(0,0,bw,bw),*textures[3],true));
        AGTable::addChild(2,2,new AGImage(this,AGRect2(0,0,bw,bw),*textures[5],true));

        AGTable::addChild(0,3,new AGImage(this,AGRect2(0,0,bw,bw),*textures[6],true));
        AGTable::addChild(1,3,new AGImage(this,AGRect2(0,0,bw,bw),*textures[7],true));
        AGTable::addChild(2,3,new AGImage(this,AGRect2(0,0,bw,bw),*textures[8],true));

        AGTable::addChild(1,2,mClient=new AGCaption(this,AGRect2(0,0,0,0),"",theme.getFont("window.title.font"),AGBackground(theme,"window.background")));

      }
    else
      {
        // use 3 rows
        addFixedRow(bw);
        addRow(1.0);
        addFixedRow(bw);
        addFixedColumn(bw);
        addColumn(1.0);
        addFixedColumn(bw);

        AGTable::addChild(0,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[0],true));
        AGTable::addChild(1,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[1],true));
        AGTable::addChild(2,0,new AGImage(this,AGRect2(0,0,bw,bw),*textures[2],true));

        AGTable::addChild(0,1,new AGImage(this,AGRect2(0,0,bw,bw),*textures[3],true));
        AGTable::addChild(2,1,new AGImage(this,AGRect2(0,0,bw,bw),*textures[5],true));

        AGTable::addChild(0,2,new AGImage(this,AGRect2(0,0,bw,bw),*textures[6],true));
        AGTable::addChild(1,2,new AGImage(this,AGRect2(0,0,bw,bw),*textures[7],true));
        AGTable::addChild(2,2,new AGImage(this,AGRect2(0,0,bw,bw),*textures[8],true));

        AGTable::addChild(1,1,mClient=new AGCaption(this,AGRect2(0,0,0,0),"",theme.getFont("window.title.font"),AGBackground(theme,"window.background")));
      }

    arrange();
  }

AGWindow::~AGWindow() throw()
  {}

AGWidget *AGWindow::getClient()
  {
    return mClient;
  }

bool AGWindow::eventMouseButtonDown(AGEvent *e)
  {
    CTRACE;
    if(!hasFocus())
      {
        CTRACE;
        if(e->isSDLEvent())
          if(getScreenRect().contains(e->getMousePosition()))
            if(getParent())
              {
                getParent()->gainFocus(this);
                return true;
              }
      }

    bool ret=AGTable::eventMouseButtonDown(e);

    if(e->isSDLEvent())
      if(getScreenRect().contains(e->getMousePosition()))
        return true; // eat - if it was here - always!!!
    return ret;

  }

bool AGWindow::eventDragBy(AGEvent *event,const AGVector2 &pDiff)
  {
    //  TRACE;
    setTop(top()+pDiff[1]);
    setLeft(left()+pDiff[0]);
    return true;
  }

AGWidget *AGWindow::getTitleBar(int w,int h)
  {
    cdebug("W:"<<w);
    AGTable *t=new AGTable(this,AGRect2(0,0,0,20));//60,20));
    AGSurface closeSurface=getTheme()->getSurface("window.buttons.close");
    AGButton *closeButton;
    AGWidget *title;

    AGTable::addChild(1,1,t);

    t->addRow(1.0);

    t->addColumn(1.0);
    t->addFixedColumn(h);//close button

    //  t->addChild(0,0,title=new AGButton(t,AGRect2(0,0,0,0),mTitle));
    t->addChild(0,0,title=new AGCaption(t,AGRect2(0,0,0,0),mTitle,getTheme()->getFont("window.title.font"),AGBackground("window.title.background")));
    t->addChild(1,0,closeButton=new AGButton(t,AGRect2(0,0,20,20),"aa"));
    closeButton->setSurface(closeSurface,false);
    title->setName("title"); // FIXME: maybe name getName()+".title"

    //  AGListener
    closeButton->sigClick.connect(slot(this,&AGWindow::tryClose));
    //  t->arrange();
    return t;
  }

bool AGWindow::tryClose(AGEvent *m)
  {
    TRACE;
    hide();
    m->setCaller(this);
    sigClose(m);
    return false;
  }

void AGWindow::close()
  {
    hide();
  }

AGRect2 AGWindow::getClientRect() const
{
  return const_cast<AGWindow*>(this)->getClient()->getClientRect()+mClient->getRect().getV0();
}

AGWindow &toAGWindow(AGWidget &w)
  {
    return dynamic_cast<AGWindow&>(w);
  }
