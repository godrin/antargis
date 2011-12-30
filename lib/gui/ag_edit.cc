/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_edit.cc
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

#include "ag_edit.h"

#include "rk_debug.h"
#include "rk_tools.h"

#include "ag_fontengine.h"
#include "ag_theme.h"
#include "ag_menu.h"
#include "ag_sgeexport.h"
#include "ag_glscreen.h"


AGEditLine::AGEditLine(const AGStringUtf8 &pText,AGFont pFont,bool pHardEnd):
  mText(pText),mFont(pFont),mHardEnd(pHardEnd)
    {
      mAlign=EDIT_LEFT;
      mVAlign=EDIT_TOP;
    }

AGEditLine::~AGEditLine()
  {
  }

void AGEditLine::setAlign(AGAlign pAlign)
  {
    mAlign=pAlign;
  }

void AGEditLine::setVAlign(AGVAlign pVAlign)
  {
    mVAlign=pVAlign;
  }

void AGEditLine::setFont(const AGFont &pFont)
  {
    mFont=pFont;
  }


AGFont AGEditLine::getFont() const
{
  return mFont;
}

#define SMALL_CHUNKS

void AGEditLine::draw(AGPainter &p,const AGVector2 &pPoint,const AGRect2 &pClip)
  {
    int x=0;
    if(mAlign==EDIT_CENTER)
      x=(int)((pClip.w()-mFont.getWidth(mText))/2);

#ifdef SMALL_CHUNKS
    int mx=0;
    std::vector<AGStringUtf8> a=mText.split(AGStringUtf8(" "));
    for(std::vector<AGStringUtf8>::iterator i=a.begin();i!=a.end();i++)
      {
        //      cdebug(*i);
        p.renderText(*i,AGVector2(pPoint[0]+x+mx,pPoint[1]),mFont);
        mx+=getScreen().getFontEngine()->getWidth(mFont,*i+" ");
      }
#else
    p.renderText(mText+(mHardEnd?"":""),AGVector2(pPoint.x+x,pPoint.y),mFont);
#endif
  }

void AGEditLine::drawCursor(AGPainter &p,int cx,const AGVector2 &pPoint,const AGRect2 &pClip,const AGColor &c)
  {
    int x1=getScreen().getFontEngine()->getWidth(mFont,mText.substr(0,cx));
    int x2=getScreen().getFontEngine()->getWidth(mFont,mText.substr(0,cx+1));

    int w=x2-x1;
    if(w==0)
      w=8;

#ifdef SIMPLE_BOX_CURSOR
    p.fillRect(AGRect2(pPoint[0]+x1,pPoint[1],w,height()),c);
#else
    float x=pPoint[0]+x1-4;
    float y=pPoint[1];
    float h=height();

    w=10;

    p.fillRect(AGRect2(x,y,w+1,3),c);
    p.fillRect(AGRect2(x+w/2-1,y+3,3,h-6),c);
    p.fillRect(AGRect2(x,y+h-3,w+1,3),c);
#endif
  }


int AGEditLine::height() const
{
  return getScreen().getFontEngine()->getHeight(mFont,mText);
}
int AGEditLine::width() const
{
  return getScreen().getFontEngine()->getWidth(mFont,mText);
}

void AGEditLine::insert(const AGStringUtf8 &c,int cx,bool pInsert)
  {
    if(pInsert)
      mText=mText.substr(0,cx)+c+mText.substr(cx,AGString::npos);
    else if(cx<(long)mText.length())
      mText[cx]=c;
    else
      mText+=c;
  }

void AGEditLine::doDelete(int cx)
  {
    if(mText.length()>0)
      if(cx>=0&&cx<int(mText.length()))
        mText=mText.substr(0,cx)+mText.substr(cx+1,AGString::npos);

  }

AGEditLine AGEditLine::split(int cx)
  {
    AGStringUtf8 n=mText.substr(cx,mText.npos);
    mText=mText.substr(0,cx);
    bool hard=mHardEnd;
    mHardEnd=true;
    return AGEditLine(n,mFont,hard);
  }

int AGEditLine::length() const
{
  return mText.length();
}

void AGEditLine::append(const AGStringUtf8 &s)
  {
    mText+=s;
  }

void AGEditLine::prepend(const AGStringUtf8 &s)
  {
    mText=s+mText;
  }
AGStringUtf8 AGEditLine::getText() const
{
  return mText;
}

// returns the remaing string after wrapping
std::pair<AGStringUtf8,bool> AGEditLine::checkWrap(int pW)
  {
    // first check, if line is too long

    if(getScreen().getFontEngine()->getWidth(mFont,mText)<pW)
      return std::make_pair("",false);

    // ok, line is too long
    // so search for a good split (between words), but not before half of width
    std::vector<AGStringUtf8> words=mText.split(" ");

    std::vector<AGStringUtf8>::iterator i=words.begin();
    int w=0,ow=0;
    AGStringUtf8 s,os;

    for(;i!=words.end();i++)
      {
        s+=*i;
        w=getScreen().getFontEngine()->getWidth(mFont,s);
        if(w>pW)
          break;

        ow=w;
        os=s;
        s+=" ";
      }
    if(ow>pW/4 && ow<width())
      {
        // check if width will be at least a 1/4 of whole width
        AGStringUtf8 n=mText.substr(os.length()+1,n.npos);
        mText=mText.substr(0,os.length()+1);

        bool hard=mHardEnd;
        mHardEnd=false;
        return std::make_pair(n,hard);
      }


    // find first character, which stands over
    size_t k=0;
    w=ow=0;
    s=os="";
    for(;k<mText.length();k++)
      {
        s+=mText.substr(k,1);
        w=getScreen().getFontEngine()->getWidth(mFont,s);
        if(w>pW)
          break;

        ow=w;
        os=s;
      }
    if(k>=mText.length())
      return std::make_pair("",false); // some error
    AGStringUtf8 n=mText.substr(k,mText.length());
    mText=mText.substr(0,k);
    bool hard=mHardEnd;
    mHardEnd=false;
    return std::make_pair(n,hard);
  }

void AGEditLine::setHardEnd(bool pHardEnd)
  {
    mHardEnd=pHardEnd;
  }

bool AGEditLine::hardEnd() const
{
  return mHardEnd;
}

std::pair<AGStringUtf8,bool> AGEditLine::checkUnwrap(int pW,const AGStringUtf8 &s)
  {
    AGStringUtf8 oldtext=mText;
    //  mText+=" ";
    mText+=s;
    AGStringUtf8 testtext=mText;
    std::pair<AGStringUtf8,bool> res=checkWrap(pW);
    if(mText!=oldtext)
      return std::make_pair(testtext.substr(mText.length(),AGString::npos),true);
    else
      return std::make_pair("",false);

  }

void AGEditLine::setText(const AGStringUtf8 &s)
  {
    mText=s;
  }







AGEdit::AGEdit(AGWidget *pParent,const AGRect2 &pRect):
  AGWidget(pParent,pRect),mCursorTime(0),mCursorLast(SDL_GetTicks()),
  mLShift(false),mRShift(false),mLCtrl(false),mRCtrl(false),mLAlt(false),mRAlt(false),
  mMultiLine(true),mWrapLines(true),
  sigModified(this,"sigModified")
    {
      mInserting=true;
      mMaxLength=-1;
      mMutable=true;
      mShowCursor=true;
      AGFont font1("FreeSans.ttf",14);
      AGFont font2("FreeSans.ttf",13);
      font1.setColor(AGColor(0,0,0));
      font2.setColor(AGColor(0,0,0xFF));
      AGEditLine l("",font1,true);
      mAlign=EDIT_LEFT;
      mVAlign=EDIT_TOP;
      l.setAlign(mAlign);
      l.setVAlign(mVAlign);
      mLines.push_back(l);

      mCx=mCy=0;
      mViewCy=0;

      AGString t=mTheme;
      if(t.length())
        t+=".";
      mBackground=AGBackground(t+"edit.background");
      mDrawBackground=true;
      setTheme("edit");
    }

AGEdit::~AGEdit()  throw()
  {}


void AGEdit::setMaxLength(int i)
  {
    mMaxLength=i;
  }


void AGEdit::draw(AGPainter &p)
  {
    //    cdebug("text:"<<getText());
    int x,y,cy;
    int completeHeight=0;
    x=y=cy=0;

    drawBackground(p);

    std::list<AGEditLine>::iterator i=mLines.begin();

    AGRect2 mr(getRect());

    AGColor cursorC=AGColor(0,0,0,0x7f);
    if(mShowCursor) // && opengl())
      cursorC=AGColor(0x7f,0x7f,0x7f,0x7f);

    if(mCy<mViewCy)
      mViewCy=mCy;

    // get complete height - for hor. centering
    for(i=mLines.begin();i!=mLines.end();i++)
      {
        completeHeight+=i->height();
      }

    if(mVAlign==EDIT_VCENTER)
      y=(int)(getRect().h()/2-completeHeight/2);

    i=mLines.begin();

    for(int k=0;k<mViewCy;k++)
      i++;

    for(;i!=mLines.end();i++)
      {
        i->draw(p,AGVector2(x,y),getRect().origin());
        if(cy+mViewCy==mCy && mMutable && hasFocus())
          i->drawCursor(p,mCx,AGVector2(x,y),getRect(),cursorC);
        y+=i->height();
        if(y>getRect().h())
          {
            break;
          }
        cy++;
        if(!mMultiLine)
          {
            break;
          }
      }

    // do it next time
    if(cy+mViewCy-1<mCy)
      mViewCy=mCy-cy+1;

  }

void AGEdit::drawBackground(AGPainter &p)
  {
    //  CTRACE;
    if(mDrawBackground)
      {
        //  CTRACE;
        mBackground.draw(getRect().origin(),p);
      }
    //    p.tile(mBackground,getRect());
  }

bool AGEdit::eventKeyUp(AGEvent *m)
  {
    if(!mMutable)
      return false;
    //  const AGSDLEvent *m=reinterpret_cast<const AGSDLEvent*>(m2);
    if(m->isSDLEvent())
      {
        SDLKey k=m->getKey();
        //      char ins=0;
        //      bool doInsert=false;
        if(k==SDLK_LSHIFT)
          mLShift=false;
        else if(k==SDLK_RSHIFT)
          mRShift=false;
        else if(k==SDLK_LCTRL)
          mLCtrl=false;
        else if(k==SDLK_RCTRL)
          mRCtrl=false;
        else if(k==SDLK_LALT)
          mLAlt=false;
        else if(k==SDLK_RALT)
          mRAlt=false;
      }
    return false;
  }


bool AGEdit::processKey(SDLKey k,Uint16 unicode)
  {
    AGStringUtf8 ins;
    bool doInsert=false;
    bool used=false;

    cdebug(k<<"::"<<SDLK_RETURN);
    if(k==SDLK_RIGHT)
      {
        getActLine();
        if(actLine->length()>mCx)
          mCx++;
        else if(mCy<(int)mLines.size()-1)
          {
            mCy++;
            mCx=0;
          }
        return true;
      }
    else if(k==SDLK_LEFT)
      {
        if(mCx>0)
          mCx--;
        else if(mCy>0)
          {
            mCy--;
            getActLine();
            mCx=actLine->length();
          }

        return true;
      }
    else if(k==SDLK_UP && mMultiLine)
      {
        if(mCy>0)
          mCy--;
        getActLine();
        if(mCx>=actLine->length())
          mCx=actLine->length();
        return true;
      }
    else if(k==SDLK_DOWN && mMultiLine)
      {
        if((int)mLines.size()>mCy+1)
          mCy++;
        getActLine();
        if(mCx>=actLine->length())
          mCx=actLine->length();
        return true;
      }
    else if(k==SDLK_SPACE)
      {
        doInsert=true;
        ins=" ";
      }
    else if(k==SDLK_BACKSPACE)
      {
        if(mCx>0)
          {
            doDelete(mCx-1);
            mCx--;
          }
        else if(mCy>0 && mMultiLine)
          {
            mCy--;
            getActLine();
            mCx=actLine->length();
            if(!actLine->hardEnd())
              {
                // delete last char
                doDelete(actLine->length()-1);
                mCx--;
              }
            mergeLine(mCy);
          }
        checkWrap();
        used=true;
      }
    else if(k==SDLK_DELETE)
      {
        doDelete(mCx);
        checkWrap();
        used=true;
      }
    else if(k==SDLK_RETURN)
      {
        cdebug("RETUURN!");
        cdebug(mMultiLine);
        if(mMultiLine)
          {
            getActLine();
            AGEditLine l=actLine->split(mCx);
            l.setAlign(mAlign);
            l.setVAlign(mVAlign);
            mCx=0;
            mCy++;
            insertLine(l);
            checkWrap();
          }
        used=true;
      }
    else if(k==SDLK_END)
      {
        getActLine();
        mCx=actLine->length();
        used=true;
      }
    else if(k==SDLK_HOME)
      {
        mCx=0;
        used=true;
      }

    else if(k==SDLK_LSHIFT)
      {
        used=true;
        mLShift=true;
      }
    else if(k==SDLK_RSHIFT)
      {
        used=true;
        mRShift=true;
      }
    else if(k==SDLK_LCTRL)
      {
        used=true;
        mLCtrl=true;
      }
    else if(k==SDLK_RCTRL)
      {
        used=true;
        mRCtrl=true;
      }
    else if(k==SDLK_LALT)
      {
        cdebug("lalt");
        mLAlt=true;
      }
    else if(k==SDLK_RALT)
      {
        cdebug("ralt");
        mRAlt=true;
      }
    else if(k==SDLK_ESCAPE)
      {}
    else if(k==SDLK_DOWN)
      {}
    else if(k==SDLK_UP)
      {}
    else if((unicode&0xFFF8)!=0 || (k>=SDLK_0 && k<=SDLK_9) || (k>=SDLK_a && k<=SDLK_z))
      {
        ins=AGStringUtf8(unicode2Utf8(unicode));
        cdebug((int)ins.toString()[0]<<"  "<<(int)ins.toString()[1]);

        doInsert=true;
      }
    cdebug("KEY:"<<SDL_GetKeyName(k)<<"  "<<k);
    cdebug("INSERT:"<<doInsert);
    if(doInsert)
      {
        if(insert(ins))
          mCx++;
        checkWrap();
        return true;
      }
    if(used)
      return true;
    return false;
  }

bool AGEdit::eventKeyDown(AGEvent *m)
  {
    if(!mMutable)
      return false;
    if(!hasFocus())
      return false;
    queryRedraw();
    if(m->isSDLEvent())
      {
        SDLKey k=m->getKey();
        Uint16 unicode=m->getUnicode();

        bool result=processKey(k,unicode);
        if(result)
          {
            sigModified(m);
          }

        return result;
      }
    return false;
  }

bool AGEdit::insert(const AGStringUtf8 &c)
  {
    if(mMaxLength>0)
      {
        if((int)getText().length()==mMaxLength)
          {
            if(mInserting || mCx==actLine->length())
              return false; // ignore input
          }
      }
    getActLine(); // FIXME:try to cache
    actLine->insert(c,mCx,mInserting);
    return true;
  }

void AGEdit::getActLine()
  {
    // slow
    int i=0;
    std::list<AGEditLine>::iterator j=mLines.begin();

    for(;i!=mCy;i++)
      j++;
    actLine=&(*j);
  }

void AGEdit::doDelete(int p)
  {
    if(p>=0)
      {
        getActLine();
        if(p==(int)actLine->length() && mCy<(int)mLines.size()-1)
          {
            // merge 2 lines
            mergeLine(mCy);
          }
        else
          actLine->doDelete(p);

      }
  }

void AGEdit::insertLine(AGEditLine l)
  {
    std::list<AGEditLine>::iterator j=mLines.begin();
    for(;&(*j)!=actLine;j++);
    j++;
    mLines.insert(j,l);
  }

void AGEdit::mergeLine(int p)
  {
    if(!mMultiLine)
      return;
    std::list<AGEditLine>::iterator j=mLines.begin();
    int i=0;
    for(;i<p;i++)
      j++;
    actLine=&(*j);
    j++;
    actLine->append(j->getText());
    bool h=j->hardEnd();
    mLines.erase(j);
    actLine->setHardEnd(h);
  }

void AGEdit::setMulti(bool pMultiLine)
  {
    mMultiLine=pMultiLine;
  }

void AGEdit::checkWrap()
  {
    if(mWrapLines)
      {
        std::list<AGEditLine>::iterator i=mLines.begin();
        for(;i!=mLines.end();)
          {
            std::pair<AGStringUtf8,bool> n=i->checkWrap((int)width());
            if(n.first.length())
              {
                // make new line
                if(i->length()<mCx)
                  {
                    mCx-=i->length();
                    mCy++;
                  }
                i++;
                if(n.second) // is hard end
                  {
                    AGEditLine l(n.first,actLine->getFont(),n.second);
                    l.setAlign(mAlign);
                    l.setVAlign(mVAlign);
                    i=mLines.insert(i,l);
                  }
                else
                  {
                    if(i==mLines.end())
                      {
                        AGEditLine l(n.first,actLine->getFont(),n.second);
                        l.setAlign(mAlign);
                        l.setVAlign(mVAlign);
                        mLines.push_back(l);
                        i=mLines.end();
                        i--;
                      }
                    else
                      {
                        // fill line after
                        i->prepend(n.first);
                      }
                  }

              }
            else
              i++;
          }

        // unwrapping -test

        int cy=0;
        i=mLines.begin();
        for(;i!=mLines.end();)
          {
            std::list<AGEditLine>::iterator j=i;
            j++;
            if(j!=mLines.end() && !i->hardEnd())
              {
                std::pair<AGStringUtf8,bool> nText=i->checkUnwrap((int)width(),j->getText());
                if(nText.second) // changed
                  {
                    int count=j->getText().length()-nText.first.length();
                    // do nothing
                    j->setText(nText.first);

                    // correct cursor position
                    if(cy==mCy-1)
                      {
                        // cursor is in next line
                        mCx-=count;
                        if(mCx<0)
                          {
                            mCy--;
                            mCx+=i->length();
                          }
                      }
                    else if(cy==mCy)
                      {
                        if(mCx>i->length())
                          {
                            mCy++;
                            mCx-=i->length();
                          }
                      }
                    if(nText.first=="")
                      {
                        // empty line
                        i->setHardEnd(j->hardEnd());
                        mLines.erase(j);
                      }
                  }
                else
                  {
                    i++;
                    cy++;
                  }
              }
            else
              {
                i++;
                cy++;
              }
          }

      }
  }


void AGEdit::setFont(const AGFont &pFont)
  {
    mFont=pFont;
    std::list<AGEditLine>::iterator i=mLines.begin();
    for(;i!=mLines.end();i++)
      i->setFont(mFont);

  }

void AGEdit::setText(const AGStringUtf8 &pText)
  {
    if(getText()==pText)
      return;

    queryRedraw();

    clear();

    size_t i;
    for(i=0;i<pText.length();i++)
      {
        if(pText[i]=='\n')
          {
            getActLine();
            AGEditLine l=actLine->split(mCx);
            l.setAlign(mAlign);
            l.setVAlign(mVAlign);
            mCx=0;
            mCy++;
            insertLine(l);
            checkWrap();
          }
        else
          {
            insert(AGStringUtf8(pText[i]));
            mCx++;
            checkWrap();
          }
      }
    setFont(mFont); // reset Font
    checkWrap();
    //  mCy=0;
    //  mCx=mT
    setFont(mFont); // reset Font
  }
void AGEdit::setMutable(bool pMutable)
  {
    mMutable=pMutable;
    mShowCursor=pMutable; // FIXME: show cursor only if widget has focus
  }

void AGEdit::setAlign(AGAlign pAlign)
  {
    mAlign=pAlign;
    std::list<AGEditLine>::iterator i=mLines.begin();
    for(;i!=mLines.end();i++)
      i->setAlign(pAlign);
  }

void AGEdit::setVAlign(AGVAlign pVAlign)
  {
    mVAlign=pVAlign;
    std::list<AGEditLine>::iterator i=mLines.begin();
    for(;i!=mLines.end();i++)
      i->setVAlign(pVAlign);
  }

void AGEdit::setBackground(const AGBackground &bg)
  {
    mDrawBackground=true;
    mBackground=bg;
  }


void AGEdit::setBackground(bool pDrawBackground)
  {
    queryRedraw();
    mDrawBackground=pDrawBackground;
  }

void AGEdit::setTheme(const AGString &s)
  {
    queryRedraw();
    AGString ms=s;
    if(ms=="")
      ms="edit";

    AGFont font=getTheme()->getFont(s+".font");
    setFont(font);
    mTheme=s;

    AGString t=mTheme;
    if(t.length())
      t+=".";

    mBackground=AGBackground(t+"background");
  }

bool AGEdit::canFocus() const
{
  return mMutable;
}

AGEdit &toAGEdit(AGWidget &w)
  {
    return dynamic_cast<AGEdit&>(w);
  }

AGStringUtf8 AGEdit::getText() const
{
  std::ostringstream os;
  std::list<AGEditLine>::const_iterator i=mLines.begin();
  for(;i!=mLines.end();)
    {
      bool hard=i->hardEnd();
      os<<i->getText().toString();
      i++;
      if(i!=mLines.end() && hard)
        os<<std::endl;
    }
  return AGStringUtf8(os.str());
}

void AGEdit::clear() throw()
  {
    mLines.clear();
    mCx=mCy=0;
    mViewCy=0;
    AGFont font1("FreeSans.ttf",14);
    AGEditLine l("",font1,true);
    l.setAlign(mAlign);
    l.setVAlign(mVAlign);
    mLines.push_back(l);
    getActLine();
  }


bool AGEdit::eventGotFocus()
  {
    queryRedraw();
    return AGWidget::eventGotFocus();
  }
bool AGEdit::eventLostFocus()
  {
    queryRedraw();
    return AGWidget::eventLostFocus();
  }

void AGEdit::prepareDraw()
  {
    mCursorTime+=SDL_GetTicks()-mCursorLast;
    mCursorLast=SDL_GetTicks();
    if(mCursorTime>300 && mMutable && hasFocus())
      {
        mCursorTime=0;
        mShowCursor=!mShowCursor;
        queryRedraw();
      }


    AGWidget::prepareDraw();
  }

void AGEdit::setInsert(bool pInsert)
  {
    mInserting=pInsert;
  }

/// set Cursor to defined position p in text
void AGEdit::setCursor(int p)
  {
    std::list<AGEditLine>::iterator j=mLines.begin();

    mCy=0;
    mCx=0;
    for(;j->length()<p;)
      {
        mCy++;
        p-=j->length();
        j++;
      }
    mCx=p;
    actLine=&(*j);

  }
