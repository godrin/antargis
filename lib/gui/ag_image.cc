/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_image.cc
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

#include "ag_image.h"
#include "rk_debug.h"

AGImage::AGImage(AGWidget *pParent,const AGRect2 &r,AGSurface pSurface,bool pTile):
  AGWidget(pParent,r),
  mTexture(pSurface),mTile(pTile),mScale(false)
    {
      mCenter=true;
    }

AGImage::AGImage(AGWidget *pParent,const AGRect2 &r,AGTexture pTexture,bool pTile):
  AGWidget(pParent,r),
  mTexture(pTexture),mTile(pTile),mScale(false)
  {
    mCenter=true;
  }



AGImage::~AGImage() throw()
  {
  }

void AGImage::draw(AGPainter &p)
  {
    // do always center for now
    bool center=true;


    if(mTile)
      {
        p.tile(mTexture,getRect().origin());
      }
    else if(center)
      {

        AGRect2 mr=getRect().origin();
        AGRect2 fr=mTexture.getRect();

        if(mCenter && !mScale)
          mr+=AGVector2((width()-mTexture.width())/2,(height()-mTexture.height())/2);

        if(mScale)
          p.blit(mTexture,mr,fr);
        else
          p.blit(mTexture,mr);
      }
    else
      p.blit(mTexture,getRect().origin());
  }

void AGImage::setSurface(const AGSurface &pSurface)
  {
    AGTexture t(pSurface);
    mTexture=t;
    queryRedraw();
  }
void AGImage::setTexture(const AGTexture &pTexture)
  {
    mTexture=pTexture;
    queryRedraw();
  }

void AGImage::useTextures()
  {
    mTexture.useTexture();
  }

void AGImage::setCenter(bool c)
  {
    mCenter=c;
  }

void AGImage::setScale(bool c)
  {
    mScale=c;
  }
