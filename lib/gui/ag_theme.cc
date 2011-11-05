/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_theme.cc
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

#include "ag_theme.h"

#include "rk_debug.h"
#include "rk_tools.h"

#include "ag_xml.h"
#include "ag_config.h"

#include <iostream>
using namespace std;

AGTheme *mTheme=0;

AGTheme::AGTheme()
  {
  }

AGTheme::~AGTheme() throw()
  {
  }

void AGTheme::setFont(const AGString &pName,AGFont pFont)
  {
    mFonts[pName]=pFont;
  }
void AGTheme::setColor(const AGString &pName,AGColor pColor)
  {
    mColors[pName]=pColor;
  }


AGFont AGTheme::getFont(const AGString &pName) const
{
  std::map<AGString,AGFont>::const_iterator i=mFonts.find(pName);

  if(i==mFonts.end())
    {
      i=mFonts.find(trunc(pName));
      if(i==mFonts.end())
        return AGFont();
    }
  return i->second;
}
AGColor AGTheme::getColor(const AGString &pName) const
{
  std::map<AGString,AGColor>::const_iterator i=mColors.find(pName);

  if(i==mColors.end())
    {
      AGString n=trunc(pName);
      i=mColors.find(n);
      if(i==mColors.end())
        return AGColor();
    }
  return i->second;
}

AGString AGTheme::trunc(AGString s) const
  {
    size_t i=s.find(".");
    if(i!=s.npos)
      s=s.substr(0,i);
    return s;
  }

int AGTheme::getInt(const AGString &pName) const
{
  std::map<AGString,int>::const_iterator i=mInts.find(pName);

  if(i==mInts.end())
    {
      AGString n=trunc(pName);
      i=mInts.find(n);
      if(i==mInts.end())
        return 0;
    }
  return i->second;
}
void AGTheme::setInt(const AGString &pName,int i)
  {
    mInts[pName]=i;
  }



void loadDefaultTheme()
  {
    loadThemeFile("theme.xml");

    if(!mTheme)
      mTheme=new AGTheme;
  }

void setTheme(const AGTheme &t)
  {
    if(mTheme)
      checkedDelete(mTheme);
    mTheme=new AGTheme(t);
  }

AGTheme *getTheme()
  {
    if(mTheme==0)
      loadDefaultTheme();
    return mTheme;
  }

bool AGTheme::hasSurface(const AGString &pName) const
{
  return(mSurfaces.find(pName)!=mSurfaces.end());
}
bool AGTheme::hasSurfaceName(const AGString &pName) const
{
  return(mSurfaceNames.find(pName)!=mSurfaceNames.end());
}
bool AGTheme::hasInt(const AGString &pName) const
{
  return(mInts.find(pName)!=mInts.end());
}
bool AGTheme::hasColor(const AGString &pName) const
{
  return(mColors.find(pName)!=mColors.end());
}
bool AGTheme::hasFont(const AGString &pName) const
{
  return(mFonts.find(pName)!=mFonts.end());
}


AGSurface AGTheme::getSurface(const AGString &pName) const
  {
    std::map<AGString,AGSurface>::const_iterator i=mSurfaces.find(pName);
    if(i==mSurfaces.end())
      {
        i=mSurfaces.find(trunc(pName));
        if(i==mSurfaces.end())
          return AGSurface();
      }
    return i->second;
  }
void AGTheme::setSurface(const AGString &pName,const AGSurface &pSurface)
  {
    assert(pSurface.valid());
    mSurfaces[pName]=pSurface;
    assert(mSurfaces[pName].valid());
  }

std::string AGTheme::getSurfaceName(const AGString &pName) const
  {
    std::map<AGString,std::string>::const_iterator i=mSurfaceNames.find(pName);
    if(i==mSurfaceNames.end())
      {
        i=mSurfaceNames.find(trunc(pName));
        if(i==mSurfaceNames.end())
          return "";
      }
    return i->second;
  }
void AGTheme::setSurfaceName(const AGString &pName,const std::string &pSurface)
  {
    mSurfaceNames[pName]=pSurface;
  }

AGLocalTheme AGTheme::getTheme(const AGString &pTheme) const
{
  return AGLocalTheme(pTheme);
}

AGGradient AGTheme::getGradient(const AGString &pName) const
{
  std::map<AGString,AGGradient>::const_iterator i=mGradients.find(pName);
  if(i==mGradients.end())
    {
      i=mGradients.find(trunc(pName));
      if(i==mGradients.end())
        return AGGradient();
    }
  return i->second;
}

AGBackground AGTheme::getBackground(const AGString &pName) const
{
  std::map<AGString,AGBackground>::const_iterator i=mBackgrounds.find(pName);
  if(i==mBackgrounds.end())
    {
      i=mBackgrounds.find(trunc(pName));
      if(i==mBackgrounds.end())
        return AGBackground();
    }
  return i->second;
}

AGBorder AGTheme::getBorder(const AGString &pName) const
{
  std::map<AGString,AGBorder>::const_iterator i=mBorders.find(pName);
  if(i==mBorders.end())
    {
      i=mBorders.find(trunc(pName));
      if(i==mBorders.end())
        return AGBorder();
    }
  return i->second;
}




AGLocalTheme::AGLocalTheme(const AGString &pTheme):mTheme(pTheme)
      {
      }

AGLocalTheme::~AGLocalTheme() throw()
  {}

AGFont AGLocalTheme::getFont(const AGString &pName) const
  {
    AGString t=getName(pName);
    if(getTheme()->hasFont(t))
      return getTheme()->getFont(t);
    return getTheme()->getFont(pName);
  }
AGColor AGLocalTheme::getColor(const AGString &pName) const
  {
    AGString t=getName(pName);
    if(getTheme()->hasColor(t))
      return getTheme()->getColor(t);
    return getTheme()->getColor(pName);
  }

int AGLocalTheme::getInt(const AGString &pName) const
  {
    AGString t=getName(pName);
    if(getTheme()->hasInt(t))
      return getTheme()->getInt(t);
    return getTheme()->getInt(pName);
  }

AGSurface AGLocalTheme::getSurface(const AGString &pName) const
{
  AGString t=getName(pName);
  if(getTheme()->hasSurface(t))
    return getTheme()->getSurface(t);
  return getTheme()->getSurface(pName);
}

std::string AGLocalTheme::getSurfaceName(const AGString &pName) const
{
  AGString t=getName(pName);
  if(getTheme()->hasSurfaceName(t))
    return getTheme()->getSurfaceName(t);
  return getTheme()->getSurfaceName(pName);
}

bool AGLocalTheme::hasFont(const AGString &pName) const
{
  return getTheme()->hasFont(getName(pName));
}
bool AGLocalTheme::hasColor(const AGString &pName) const
{
  return getTheme()->hasColor(getName(pName));
}
bool AGLocalTheme::hasInt(const AGString &pName) const
{
  return getTheme()->hasInt(getName(pName));
}
bool AGLocalTheme::hasSurface(const AGString &pName) const
{
  return getTheme()->hasSurface(getName(pName));
}
bool AGLocalTheme::hasSurfaceName(const AGString &pName) const
{
  return getTheme()->hasSurfaceName(getName(pName));
}

AGString AGLocalTheme::getName(const AGString &pName) const
{
  if(mTheme.length()>0)
    return mTheme+"."+pName;
  else
    return pName;
}








void loadTheme(const Node&node,AGTheme &t,AGString name)
  {
    Node::const_iterator i=node.begin();
    for(;i!=node.end();i++)
      {

        AGString n=name;
        if(n.length())
          n+=".";
        n+=(*i)->getName();
        if((*i)->get("name").length())
          {
            AGString sname=name+"."+(*i)->get("name");
            // read color /image whatever
            if((*i)->getName()=="color")
              t.setColor(sname,AGColor((*i)->get("color")));
            if((*i)->getName()=="image")
              {
                t.setSurface(sname,AGSurface::load((*i)->get("file")));
                t.setSurfaceName(sname,(*i)->get("file"));
              }
            if((*i)->getName()=="value")
              t.setInt(sname,(*i)->get("value").toInt());
            if((*i)->getName()=="font")
              {
                AGFont f((*i)->get("file"),(*i)->get("size").toInt());
                f.setColor(AGColor((*i)->get("color")));
                f.setEmbossed((*i)->get("embossed")=="true");
                f.setInset((*i)->get("inset")=="true");
                if((*i)->get("style")=="bold")
                  f.setStyle(AGFont::BOLD);
                f.setBorderColor(AGColor((*i)->get("borderColor")));
                f.setBorder((*i)->get("borderWidth").toInt());

                t.setFont(sname,f);
              }
          }
        else
          loadTheme(**i,t,n);
      }
  }

void loadTheme(const AGString &pXML)
  {
    AGTheme theme;

    Document p;
    p.parseMemory(pXML);

    Node n=p.root();

    loadTheme(n,theme,"");

    setTheme(theme);
  }

bool loadThemeFile(const AGString &pFilename)
  {
    AGTheme theme;

    Document p;
    if(!p.parseFile(pFilename))
      return false;

    const Node &n=p.root();

    loadTheme(n,theme,"");

    setTheme(theme);
    return true;
  }

AGString addPoint(const AGString &pTheme)
  {
    if(pTheme.length())
      if(pTheme[pTheme.length()-1]!='.')
        return pTheme+".";
    return pTheme;
  }
