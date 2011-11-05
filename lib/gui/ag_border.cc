/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_border.cc
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

#include "ag_border.h"
#include "ag_painter.h"
#include "ag_theme.h"
#include "ag_texturecache.h"

AGBorder::AGBorder(const AGString &pTheme)
  {
    mTheme=pTheme;
    AGString imageName=pTheme+".image";

    if((mEnable=getTheme()->hasSurface(imageName)))
      {
        //AGSurface surface(getTheme()->getSurface(imageName)); // not used ???
        mTexture=&getTextureCache()->get(getTheme()->getSurfaceName(imageName));
        mWidth=mTexture->width()/3;

        // build textures
        for(int y=0;y<3;y++)
          for(int x=0;x<3;x++)
            {
              AGRect2 r(x*mWidth,y*mWidth,mWidth,mWidth);
              mTextures.push_back(&getTextureCache()->get(getTheme()->getSurfaceName(imageName),r));
            }
      }
    else
      {
        mWidth=0;
        mTexture=0;
      }
  }

AGBorder::AGBorder(const AGSurface &pSurface,const AGString &pFilename)
  {
    mTexture=&getTextureCache()->get(pFilename);
    mWidth=mTexture->width()/3;

    // build textures
    for(int y=0;y<3;y++)
      for(int x=0;x<3;x++)
        {
          AGRect2 r(x*mWidth,y*mWidth,mWidth,mWidth);
          mTextures.push_back(&getTextureCache()->get(pFilename,r));
        }
    
  }

void AGBorder::draw(const AGRect2 &pd,AGPainter &p)
  {
    AGRect2 d=pd.alignGrid();
    if(mTexture==0 || !mEnable)
      return;

    if(!mTexture->hasTexture())
      {
        // FIXME: discard this and fix border-painting -problem in GL!
        mTexture=&getTextureCache()->get(getTheme()->getSurfaceName(mTheme+".image"));
      }

    int w=mTexture->width()/3;
    int w2=w*2;

    float x2=d.x1();
    float y2=d.y1();

    // skip interior


    p.tile(*mTextures[0],AGRect2(0,0,w,w));
    p.tile(*mTextures[6],AGRect2(0,y2-w,w,w));
    p.tile(*mTextures[2],AGRect2(x2-w,0,w,w));
    p.tile(*mTextures[8],AGRect2(x2-w,y2-w,w,w));

    // hor
    p.tile(*mTextures[1],AGRect2(w,0,d.w()-w2,w));
    p.tile(*mTextures[7],AGRect2(w,y2-w,d.w()-w2,w));
    // ver
    p.tile(*mTextures[3],AGRect2(0,w,w,d.h()-w2));
    p.tile(*mTextures[5],AGRect2(x2-w,w,w,d.h()-w2));


  }

size_t AGBorder::getWidth() const
{
  return mWidth;
}

void AGBorder::useTextures()
  {
    if(mTexture)
      const_cast<AGTexture*>(mTexture)->useTexture();

      for(std::vector<const AGTexture*>::iterator i=mTextures.begin();i!=mTextures.end();i++)
        const_cast<AGTexture*>(*i)->useTexture();

  }

