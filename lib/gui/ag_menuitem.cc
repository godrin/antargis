/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_menuitem.cc
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

#include "ag_menuitem.h"
#include "ag_draw.h"
#include "ag_theme.h"
#include "ag_image.h"
#include "ag_menu.h"

AGMenuItem::AGMenuItem(AGWidget *pParent,const AGStringUtf8 &pText):
  AGTable(pParent,AGRect2(0,0,0,0)),mText(pText),mMouseOver(false),mSelected(false)
  {
    AGFont font(getTheme()->getFont("Font.menu"));


    //  adaptWidthFromChildren(); // so that parent can adapt to this

    //  addChild(new AGText(this,AGVector2(0,0),"X",AGFont("Arial.ttf",25))); // test
    AGSurface surface=getTheme()->getSurface("menu.item.image");//getScreen().loadSurface("/home/david/projects/oldantargis/graphics/menu_house_small.png");

    addFixedRow(surface.height());
    addFixedColumn(surface.width());
    addColumn(1);
    arrange();

    addChild(0,0,new AGImage(this,getRect().origin(),surface,false));
    addChild(1,0,new AGText(this,getRect().origin(),pText,font));

  }
AGMenuItem::~AGMenuItem() throw()
  {
  }

void AGMenuItem::draw(AGPainter &p)//const AGRect2 &pr)
  {
    //  AGRect2 r=pr.project(getRect());

    AGColor c1,c2,c3,c4;

    AGString style=".menu";
    if(mSelected)//MouseOver)
      {
        style+=".lighted";

        AGTheme *theme=getTheme();

        c1=theme->getColor(AGString("gradientColor1")+style);
        c2=theme->getColor(AGString("gradientColor2")+style);
        c3=theme->getColor(AGString("gradientColor3")+style);
        c4=theme->getColor(AGString("gradientColor4")+style);

        //      AGSurface s(getScreen());
        p.drawGradient(p.getRect(),c1,c2,c3,c4);
      }


  }

bool AGMenuItem::eventMouseEnter()
  {
    mMouseOver=true;
    mSelected=true;
    eventSelect();

    AGMenu *p=dynamic_cast<AGMenu*>(getParent());
    //  cdebug(p);
    if(p)
      {
        p->eventItemSelected(mText.toString());
      }
    return false;
  }

bool AGMenuItem::eventMouseLeave()
  {
    mMouseOver=false;
    return false;
  }

AGString AGMenuItem::getName() const
{
  return mText.toString();
}

bool AGMenuItem::getSelected() const
{
  return mSelected;
}
void AGMenuItem::unSelect()
  {
    mSelected=false;
    eventUnselect();
  }

void AGMenuItem::eventSelect()
  {
  }
void AGMenuItem::eventUnselect()
  {
  }

bool AGMenuItem::eventMouseClick(AGEvent *m)
  {
    AGMenu *me=dynamic_cast<AGMenu*>(getParent());
    if(me)
      {
        me->eventItemClicked(mText.toString());

        return true;
      }
    return false;
  }

// AGSubMenu

AGSubMenu::AGSubMenu(AGWidget *pParent,const AGStringUtf8 &pText):
  AGMenuItem(pParent,pText)
  {
    AGSurface surface2=AGSurface::load("right_arrow.png");
    addChildBack(new AGImage(this,getRect().origin(),surface2,false));

    addChild(mSubMenu=new AGMenu(this,AGVector2(0,0),pText));
    mSubMenu->hide();
  }

AGSubMenu::~AGSubMenu() throw()
  {
  }

AGMenu *AGSubMenu::getMenu()
  {
    return mSubMenu;
  }

void AGSubMenu::addChild(AGWidget*pWidget)
  {
    AGWidget::addChild(pWidget);
  }

void AGSubMenu::eventSelect()
  {
    AGRect2 r(getScreenRect());

    getMenu()->show(r.getV10());
  }
void AGSubMenu::eventUnselect()
  {
    getMenu()->hide();
  }

bool AGSubMenu::eventMouseClick(AGEvent *m)
  {
    return false;
  }
