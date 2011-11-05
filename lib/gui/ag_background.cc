/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_background.cc
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

#include "ag_background.h"
#include "ag_draw.h"
#include "ag_theme.h"
#include "rk_debug.h"
#include "ag_painter.h"
#include "ag_texturecache.h"

/** sets a uniform background of color pColor
    @param pColor an rgba-color. so you can use transparent backgrounds,too.
 */
AGBackground::AGBackground(const AGColor &pColor):mTexture(0)
  {
    //  CTRACE;
    mColor=true;
    mColors[0]=pColor;
    mColors[1]=pColor;
    mColors[2]=pColor;
    mColors[3]=pColor;
    mBorder=0;
  }


/**
   This constructor set a themed background.
   @param pThemeName a typical string is "button.background.normal". Then the entry in the theme.xml file should look something like:
<pre>
 <button>
   <background>
     <normal>
       <color name='gradientColor1' color='#12141c'/>
       <color name='gradientColor2' color='#12141c'/>
       <color name='gradientColor3' color='#12141c'/>
       <color name='gradientColor4' color='#12141c'/>
     </normal>
   </background>
  </button>
</pre>
 */

AGBackground::AGBackground(const AGString &pThemeName):mTexture(0)
  {
    AGTheme *theme=getTheme();
    loadFromTheme(theme->getTheme(""),pThemeName);
  }

AGBackground::AGBackground(const AGLocalTheme &pTheme,const AGString &pThemeName):mTexture(0)
  {
    loadFromTheme(pTheme,pThemeName);
  }
void AGBackground::loadFromTheme(const AGLocalTheme &pTheme,const AGString &pThemeName)
  {
    mColor=false;

    if(pTheme.hasSurface(pThemeName+".image"))
      {
        cdebug(pThemeName+".image");
        mTexture=&getTextureCache()->get(pTheme.getSurfaceName(pThemeName+".image"));
      }
    else if(pTheme.hasColor(pThemeName+"."+"gradientColor1"))
      {
        mColor=true;
        mColors[0]=pTheme.getColor(pThemeName+"."+"gradientColor1");
        mColors[1]=pTheme.getColor(pThemeName+"."+"gradientColor2");
        mColors[2]=pTheme.getColor(pThemeName+"."+"gradientColor3");
        mColors[3]=pTheme.getColor(pThemeName+"."+"gradientColor4");
      }
    else if(pTheme.hasColor(pThemeName+"."+"color"))
      {
        mColor=true;
        mColors[0]=pTheme.getColor(pThemeName+"."+"color");
        mColors[1]=pTheme.getColor(pThemeName+"."+"color");
        mColors[2]=pTheme.getColor(pThemeName+"."+"color");
        mColors[3]=pTheme.getColor(pThemeName+"."+"color");
      }

    mBorder=pTheme.getInt(pThemeName+"."+"border");
  }


/// draws the background on painter in the given rectangle
void AGBackground::draw(const AGRect2 &r,AGPainter &p)
  {
    assert(&p);
    assert(&r);
    if(mTexture)
      {
        cdebug("painter:"<<&p);
        cdebug(mTexture);
        AGRect2 s=r.shrink(mBorder);
        const AGTexture &t=*mTexture;
        p.tile(t,s);//*mTexture,r.shrink(mBorder));
      }
    else if(mColor)
      p.drawGradient(r.shrink(mBorder),mColors[0],mColors[1],mColors[2],mColors[3]);

  }

/// this is a help function, so that once made textures aren't automatically discarded.
void AGBackground::useTextures()
  {
    if(mTexture)
      const_cast<AGTexture*>(mTexture)->useTexture();
  }
