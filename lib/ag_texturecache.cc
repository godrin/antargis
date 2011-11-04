/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_texturecache.cpp
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

#include "ag_texture.h"
#include "ag_texturecache.h"
#include "ag_png.h"
#include "ag_kill.h"
#include "ag_geometry.h"

AGTextureCache *mTextureCache=0;
AGTextureCache *getTextureCache()
  {
    if(!mTextureCache)
      mTextureCache=new AGTextureCache;
    return mTextureCache;
  }

AGTextureCache::AGTextureCache()
  {
    getInstanceKiller()->reg(createKiller(this));
  }

const AGTexture &AGTextureCache::get(const AGString &pTexture,const AGRect2 &pSub) throw (FileNotFound)
  {
    std::string s=pTexture+":"+pSub.toString();

    std::map<AGString,AGTexture*>::iterator i=mTextures.find(s);
    if(i==mTextures.end())
      {
        // load
        AGSurface ms=AGSurface::load(pTexture).getSubSurface(pSub);

        mTextures[s]=new AGTexture(ms,false);
      }
    return *(mTextures[s]);
  }


const AGTexture &AGTextureCache::get(const AGString &pTexture,int downScaleExp) throw (FileNotFound)
  {
    std::map<AGString,AGTexture*>::iterator i=mTextures.find(pTexture);
    if(i==mTextures.end())
      {
        // load
        AGSurface ms=AGSurface::load(pTexture);

        while(downScaleExp>1 && ms.width()>16 && ms.height()>16)
          {
            ms=ms.scale(ms.width()/2,ms.height()/2);
            downScaleExp--;
          }

        mTextures[pTexture]=new AGTexture(ms,false);
      }
    return *(mTextures[pTexture]);
  }

AGSurface skipHalfTexture(const AGSurface &s)
  {
    TRACE;
    if(s.width()>s.height())
      {
        int w=s.width();
        int h=s.height();
        int nw=w/2;
        int nh=h;
        AGSurface n(nw,nh);

        for(int i=0;i<nw/nh;i++)
          {
            for(int y=0;y<nh;y++)
              for(int x=0;x<nh;x++)
                {
                  n.putPixel(x+nh*i,y,s.getPixel(x+2*nh*i,y));
                }
          }

        return n;
      }
    else
      {
        int w=s.width();
        int h=s.height();
        int nw=w;
        int nh=h/2;
        AGSurface n(nw,nh);

        for(int i=0;i<nh/nw;i++)
          {
            for(int y=0;y<nw;y++)
              for(int x=0;x<nw;x++)
                {
                  n.putPixel(x,y+nw*i,s.getPixel(x,y+2*nw*i));
                }
          }

        return n;
      }
  }


const AGTexture &AGTextureCache::get3D(const AGString &pTexture,int downScaleExp,int downScaleZ) throw (FileNotFound)
  {
    std::map<AGString,AGTexture*>::iterator i=mTextures.find(pTexture);
    if(i==mTextures.end())
      {
        // load
        AGSurface ms=AGSurface::load(pTexture);

        cdebug("3d texture size old0:"<<ms.width()<<","<<ms.height());

        while(downScaleExp>1 && ms.width()>16 && ms.height()>16)
          {
            ms=ms.scale(ms.width()/2,ms.height()/2);
            downScaleExp--;
          }

        cdebug("3d texture size old1:"<<ms.width()<<","<<ms.height());

        while(downScaleZ>1)
          {
            ms=skipHalfTexture(ms);

            downScaleZ--;
          }

        cdebug("3d texture size:"<<ms.width()<<","<<ms.height());


        mTextures[pTexture]=new AGTexture(ms,true);
      }
    return *(mTextures[pTexture]);
  }
