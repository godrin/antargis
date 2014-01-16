/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_fontengine.cc
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

#include "ag_fontengine.h"
#include "rk_debug.h"
#include "ag_texture.h"
#include "ag_fs.h"
#include "ag_sgeexport.h"
#include "ag_profiler.h"

#include "ag_surface_internal.h"
#include "ag_screen.h"

#include <SDL_ttf.h>

#include <math.h>
#include <map>
#include <iostream>


struct FontInfo
{
  std::string pName;
  int size;

  bool operator<(const FontInfo &i) const
  {
    return pName<i.pName || (pName==i.pName && size<i.size);
  }
};

static std::map<FontInfo,TTF_Font*> fontEngineFonts;
bool fontEngineInited=false;

TTF_Font *getFont(std::string s,int i)
  {
    FontInfo info;
    info.pName=s;
    info.size=i;

    if(fontEngineFonts.find(info)!=fontEngineFonts.end())
      return fontEngineFonts[info];

    if(!fontEngineFonts[info])
      {
        TTF_Font *mFont=0;
        // init library
        if(!fontEngineInited)
          {
            fontEngineInited=true;
            if(TTF_Init()<0)
              {
                //    TRACE;
                std::cerr<<"Some error occured during initialization of libsdl-TTF"<<std::endl;
                return 0;
              }
            atexit(TTF_Quit);

          }

        // first find file
        std::string name=checkFileName(findFile(s));

        if(!mFont)
          mFont=TTF_OpenFont(name.c_str(),i);
        if(!mFont)
          {
            std::cerr<<" Font not found:"<<s<<" probable file:"<<name<<std::endl;
            std::cerr<<" Error:"<<SDL_GetError()<<std::endl;
            throw std::runtime_error(std::string("Font '")+s+"' not found!");
          }

        int renderstyle=TTF_STYLE_NORMAL;

        if(mFont)
          TTF_SetFontStyle(mFont, renderstyle);

        fontEngineFonts[info]=mFont;
      }
    return fontEngineFonts[info];
  }

AGFontEngine::AGFontEngine()
  {
    //  REGISTER_SINGLETON(this);
  }

AGFontEngine::~AGFontEngine() throw()
  {
    // TTF_Quit is called by atexit
    // font-deletion is ignored
  }


void border(AGSurface &s,AGColor bc)
  {
    float *a=new float[s.width()*s.height()];
    float *b=new float[s.width()*s.height()];

    int w=s.width();
    for(int x=0;x<s.width();x++)
      for(int y=0;y<s.height();y++)
        {
          a[y*w+x]=s.getPixel(x,y).a/float(0xFF);
        }

    float blurvalue=4;
    // twice bluring
    for(int t=0;t<8;t++)
      {
        for(int x=0;x<s.width();x++)
          for(int y=0;y<s.height();y++)
            {
              float a0,a1,a2,a3,a4;
              a0=a1=a2=a3=a4=0;
              if(y>0)
                a0=a[(y-1)*w+x];
              if(y<s.height()-1)
                a1=a[(y+1)*w+x];
              if(x>0)
                a2=a[y*w+x-1];
              if(x<s.width()-1)
                a3=a[y*w+x+1];
              a4=a[y*w+x];

              // laplacian blur
              b[y*w+x]=(a0+a1+a2+a3 + blurvalue*a4)/(blurvalue+4);
            }
        // copying
        for(int x=0;x<s.width()*s.height();x++)
          a[x]=b[x];

        // refeed !
        for(int x=0;x<s.width();x++)
          for(int y=0;y<s.height();y++)
            {
              a[y*w+x]=std::max(a[y*w+x],s.getPixel(x,y).a/float(0xFF));
            }

      }

    for(int x=0;x<s.width();x++)
      for(int y=0;y<s.height();y++)
        {
          AGColor c=s.getPixel(x,y);
          AGColor n=bc;

          n.a=((Uint8)(a[y*w+x]*0xFF));
          if(c.a>1)//0x7f)
            {
              float f=c.a/float(0xFF);
              n=n*(1-f)+c*f;
              n.a=0xFF;

            }
          s.putPixel(x,y,n);

        }

    checkedDeleteArray(a);
    checkedDeleteArray(b);
  }

void embossSurface(AGSurface &s,float depth=1.0f)
  {
    float *a=new float[s.width()*s.height()];
    float *b=new float[s.width()*s.height()];

    int w=s.width();
    for(int x=0;x<s.width();x++)
      for(int y=0;y<s.height();y++)
        {
          a[y*w+x]=s.getPixel(x,y).a/float(0xFF);
        }

    float blurvalue=12;
    // twice bluring
    for(int t=0;t<1;t++)
      {
        for(int x=0;x<s.width();x++)
          for(int y=0;y<s.height();y++)
            {
              float a0,a1,a2,a3,a4;
              a0=a1=a2=a3=a4=0;
              if(y>0)
                a0=a[(y-1)*w+x];
              if(y<s.height()-1)
                a1=a[(y+1)*w+x];
              if(x>0)
                a2=a[y*w+x-1];
              if(x<s.width()-1)
                a3=a[y*w+x+1];
              a4=a[y*w+x];

              // laplacian blur
              b[y*w+x]=(a0+a1+a2+a3 + blurvalue*a4)/(blurvalue+4);
            }
        // copying
        for(int x=0;x<s.width()*s.height();x++)
          a[x]=b[x];
      }
    //     ( 0,-1,0)
    // now (-1, 0,1) edge detection
    //     ( 0, 1,0)

    for(int x=0;x<s.width();x++)
      for(int y=0;y<s.height();y++)
        {
          float a0,a1,a2,a3;
          a0=a1=a2=a3=0;
          if(y>0)
            a0=a[(y-1)*w+x];
          if(y<s.height()-1)
            a1=a[(y+1)*w+x];
          if(x>0)
            a2=a[y*w+x-1];
          if(x<s.width()-1)
            a3=a[y*w+x+1];

          b[y*w+x]=(-a0+a1-a2+a3)*depth;
        }

    // now apply values

    for(int x=0;x<s.width();x++)
      for(int y=0;y<s.height();y++)
        {
          float v=b[y*w+x];

          v/=3;//2.0;
          v+=0.2;

            {
              AGColor c=s.getPixel(x,y);
              int olda=c.a;
              if (v>0)
                {
                  v=std::min(v,1.0f);
                  c=AGColor(0xFF,0xFF,0xFF,c.a)*v + c*(1-v);
                }
              else
                {
                  v*=-1;
                  v=std::min(v,1.0f);
                  c=AGColor(0,0,0,c.a)*v + c*(1-v);
                }
              c.a=olda;
              s.putPixel(x,y,c);
            }
        }
    checkedDeleteArray(a);
    checkedDeleteArray(b);

    /*
  static int i=0;
  i++;
  std::ostringstream os;
  os<<"fonttest"<<i<<".png";
  s.save(os.str());
     */
  }


AGTexture *AGFontEngine::renderText(int BaseLineX, int BaseLineY, const AGStringUtf8 &pText, const AGFont &pFont)
  {
    if(pText.length()==0)
      return 0;

    if(fontCache.find(std::make_pair(pFont,pText))==fontCache.end())
      {
        STACKTRACE;
        SDL_Surface *ns;
        TTF_Font *f=getFont(pFont.getName(),pFont.getSize());

        if(!f)
          return 0;

        if(pFont.getStyle()==AGFont::BOLD)
          TTF_SetFontStyle(f,TTF_STYLE_BOLD);
        else
          TTF_SetFontStyle(f,TTF_STYLE_NORMAL);

        ns=TTF_RenderUTF8_Blended(f,pText.toString().c_str(),pFont.getColor());

        assert(ns);

        SDL_Rect sr;
        sr.x=sr.y=0;
        sr.w=ns->w;
        sr.h=ns->h;

        AGInternalSurface *is=new AGInternalSurface;
        is->surface=ns;
        AGSurface as(is);

        //      if(false)
          {
            if(pFont.getBorder()>0 || pFont.getEmbossed())
              {
                int move=3;
                AGSurface copy(as.width()+2*move,as.height()+2*move);
                copy.blit(as,AGRect2(move,move,as.width(),as.height()),as.getRect(),AGColor(0xFF,0xFF,0xFF,0xFF));
                as=copy;
                border(as,pFont.getBorderColor());
              }
            else
              {
                // simply copy, so there are no more problems on PPC
                AGSurface copy(as.width(),as.height());
                copy.blit(as,as.getRect(),as.getRect(),AGColor(0xFF,0xFF,0xFF,0xFF));
                as=copy;

              }

            if(pFont.getEmbossed())
              {
                if(pFont.getInset())
                  embossSurface(as,-1);
                else
                  embossSurface(as,1);
              }
          }
          AGTexture *ms=new AGTexture(as);
          ms->setClamp(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

          fontCache[std::make_pair(pFont,pText)]=ms;
      }
    return fontCache[std::make_pair(pFont,pText)];

  }


int AGFontEngine::getWidth(const AGFont &pFont,const AGStringUtf8 &pText)
  {
    TTF_Font *f=getFont(pFont.getName(),pFont.getSize());
    if(!f)
      return 0;

    int w,h;
    int ret=TTF_SizeUTF8(f,pText.toString().c_str(),&w,&h);
    if(ret)
      return 0;
    return w;
  }

int AGFontEngine::getHeight(const AGFont &pFont,const AGStringUtf8 &pText)
  {
    TTF_Font *f=getFont(pFont.getName(),pFont.getSize());

    if(!f)
      return 0;

    int w,h;
    int ret=TTF_SizeUTF8(f,pText.toString().c_str(),&w,&h);
    if(ret)
      return 0;

    return h;
  }


  
AGFontEngine *getFontEngine() {
 return getScreen().getFontEngine();
}
