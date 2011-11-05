/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_button.cc
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

#include "ag_button.h"
#include "ag_draw.h"
#include "ag_edit.h"
#include "ag_theme.h"
#include "rk_debug.h"
#include "ag_image.h"
#include "ag_border.h"
#include "ag_glscreen.h"
#include "ag_video.h"

using namespace std;

/**
 * \brief creates a button with a text
 *
 * @param pParent the parent widget
 * @param r       relative position within the parent
 * @param pText   the caption displayed on the button. take '' for no text
 * @param id      currently not any longer used id
 */

AGButton::AGButton(AGWidget *pParent,const AGRect2 &r,const AGStringUtf8&pText,int id):
  AGWidget(pParent,r),
  mText(pText),mID(id),mState(NORMAL),mTextW(0)
  {
    CTRACE;
    mImageW=0;
    setTheme("");
    AGFont font("FreeSans.ttf");
    font.setColor(AGColor(0,0,0));
    mTextW=new AGEdit(this,r.origin().shrink(borderWidth));//,mText,font);
    mTextW->setText(mText);
    mTextW->setAlign(EDIT_CENTER);
    mTextW->setVAlign(EDIT_VCENTER);
    mTextW->setFont(font);
    mTextW->setMutable(false);
    mTextW->setBackground(false);
    addChild(mTextW);

    setTheme("");
    mChecked=false;

    mEnabled=true;

    if(opengl())
      setCaching(true);

  }
AGButton::~AGButton() throw()
  {}



void AGButton::setSurface(AGSurface pSurface,bool pChangeSize)
  {
    mSurface=pSurface;
    mGrayedSurface=mSurface.grayed();
    if(!mImageW)
      {
        mImageW=new AGImage(this,getRect().origin(),mSurface,false);
        addChild(mImageW);
      }
    else
      {
        mImageW->setSurface(pSurface);
        mImageW->show();
      }
    mTextW->hide();
    if(pChangeSize)
      {
        setWidth(mImageW->width()+2+2*borderWidth);
        setHeight(mImageW->height()+2+2*borderWidth);
      }

    else
      {
        // then center
        mImageW->setLeft((width()-mImageW->width())/2);
        mImageW->setTop((height()-mImageW->height())/2);
      }
    queryRedraw();
  }

void AGButton::setTexture(const AGTexture &pTexture)
  {
    if(!mImageW)
      {
        mImageW=new AGImage(this,getRect().origin(),mSurface,false);
        addChild(mImageW);
      }
    else
      {
        //      mImageW->setSurface(pTexture);
        mImageW->show();
      }
    mImageW->setTexture(pTexture);
  }


void AGButton::draw(AGPainter &p)
  {
    assert(mTextW);
    p.pushMatrix();
    p.transform(AGRect2(0,0,width(),height()).shrink(borderWidth));
    AGRect2 pr=getRect().origin();
    mBG[mState].draw(pr,p);
    mBorder[mState].draw(pr,p);
    p.popMatrix();

    if(borderWidth==0)
      return;

    AGColor bc1;
    AGColor bc2;
    AGTheme *theme=getTheme();

    bc1=theme->getColor(mTheme+"button.border.color1");
    bc2=theme->getColor(mTheme+"button.border.color2");

    // draw border

    AGRect2 mr;

    mr=getRect().origin();

    if(mState==NORMAL || mState==LIGHTED)
      p.drawBorder(mr,borderWidth,bc1,bc2);
    else
      p.drawBorder(mr,borderWidth,bc2,bc1);

  }


bool AGButton::eventMouseEnter()
  {
    CTRACE;
    AGWidget::eventMouseEnter();
    queryRedraw();
    if(!mEnabled)
      return false;
    if(mChecked)
      mState=CHECKEDLIGHTED;
    else
      mState=LIGHTED;
    return false;
  }
bool AGButton::eventMouseLeave()
  {
    CTRACE;
    AGWidget::eventMouseLeave();
    queryRedraw();
    if(!mEnabled)
      return false;
    if(mChecked)
      mState=CHECKED;
    else
      mState=NORMAL;
    return false;
  }


bool AGButton::eventMouseButtonDown(AGEvent *e)
  {
    if(e->isSDLEvent())
      {
        if(getScreenRect().contains(e->getMousePosition()))
          {
            if(mEnabled)
              {
                queryRedraw();
                if(mChecked)
                  mState=CHECKEDPRESSED;
                else
                  mState=PRESSED;

                AGWidget::eventMouseButtonDown(e); // let it get the buttondown-info
              }

            return true;
          }
      }
    return AGWidget::eventMouseButtonDown(e);
  }

bool AGButton::eventMouseButtonUp(AGEvent *e)
  {
    if(e->isSDLEvent())
      {
        if(mEnabled)
          {
            queryRedraw();
            if(getScreenRect().contains(e->getMousePosition()))
              {
                if(mChecked)
                  mState=CHECKEDLIGHTED;
                else
                  mState=LIGHTED;
              }
            else
              {
                if(mChecked)
                  mState=CHECKED;
                else
                  mState=NORMAL;
              }
          }

      }
    return AGWidget::eventMouseButtonUp(e);
  }


void AGButton::setRect(const AGRect2 &r)
  {
    AGWidget::setRect(r);
    updateClientRects();
  }

void AGButton::setWidth(float w)
  {
    assert(w>=0);
    AGWidget::setWidth(w);
    updateClientRects();
  }
void AGButton::setHeight(float h)
  {
    assert(h>=0);
    AGWidget::setHeight(h);
    updateClientRects();
  }

void AGButton::updateClientRects()
  {
    std::list<AGWidget*>::iterator i=mChildren.begin();
    for(;i!=mChildren.end();i++)
      (*i)->setRect(getRect().origin().shrink(borderWidth));
  }


/**
   \return returns the text displayed on the button
 */
AGStringUtf8 AGButton::getCaption() const
{
  return mText;
}

/**
   dis- or enables the button. disabled buttons are typically displayed greyed and are
   not hoverable or clckable

   \param pEnable true or false
 */

void AGButton::setEnabled(bool pEnable)
  {
    if(mEnabled!=pEnable)
      {
        mEnabled=pEnable;
        setState(mState); // check
        queryRedraw();
        if(mImageW)
          {
            if(mEnabled)
              mImageW->setSurface(mSurface);
            else
              mImageW->setSurface(mGrayedSurface);
          }

      }
  }

void AGButton::setTheme(const AGString &pTheme)
  {
    CTRACE;
    queryRedraw();
    mTheme=addPoint(pTheme);

    lower=getTheme()->getInt(mTheme+"buttonLowerOnClick");
    borderWidth=getTheme()->getInt(mTheme+"button.border.width");

    mBG[NORMAL]=AGBackground(mTheme+"button.background.normal");
    mBG[PRESSED]=AGBackground(mTheme+"button.background.pressed");
    mBG[LIGHTED]=AGBackground(mTheme+"button.background.lighted");
    mBG[CHECKED]=AGBackground(mTheme+"button.background.checked");
    mBG[CHECKEDLIGHTED]=AGBackground(mTheme+"button.background.checkedlighted");
    mBG[CHECKEDPRESSED]=AGBackground(mTheme+"button.background.checkedpressed");
    mBG[DISABLED]=AGBackground(mTheme+"button.background.disabled");

    mBorder[NORMAL]=AGBorder(mTheme+"button.border.normal");
    mBorder[PRESSED]=AGBorder(mTheme+"button.border.pressed");
    mBorder[LIGHTED]=AGBorder(mTheme+"button.border.lighted");
    mBorder[CHECKED]=AGBorder(mTheme+"button.border.checked");
    mBorder[CHECKEDLIGHTED]=AGBorder(mTheme+"button.border.checkedlighted");
    mBorder[CHECKEDPRESSED]=AGBorder(mTheme+"button.border.checkedpressed");
    mBorder[DISABLED]=AGBorder(mTheme+"button.border.disabled");

    if(mTextW)
      mTextW->setTheme(mTheme+"button.text");

  }

void AGButton::setCaption(const AGStringUtf8 &pCaption)
  {
    queryRedraw();
    mText=pCaption;
    if(mTextW)
      mTextW->setText(pCaption);
    if(mImageW)
      mImageW->hide();
  }

void AGButton::setState(const State &pState)
  {
    queryRedraw();
    if(mEnabled)
      {
        if(mState==DISABLED)
          mState=NORMAL;
        else
          mState=pState;
      }
    else
      mState=DISABLED;
  }

AGButton::State AGButton::getState() const
{
  return mState;
}


void AGButton::setChecked(bool pChecked)
  {
    queryRedraw();
    mChecked=pChecked;
    if(mChecked)
      {
        if(mState==LIGHTED || mState==CHECKEDLIGHTED)
          setState(CHECKEDLIGHTED);
        else
          setState(CHECKED);
      }
    else
      {
        if(mState==LIGHTED || mState==CHECKEDLIGHTED)
          setState(LIGHTED);
        else
          setState(NORMAL);
      }
  }
bool AGButton::isChecked() const
{
  return mChecked;
}


AGButton &toAGButton(AGWidget &pWidget)
  {
    return dynamic_cast<AGButton&>(pWidget);
  }

/**
   \brief can this widget widget be focused.

   \return always true
 */
bool AGButton::canFocus() const
{
  return true;
}

/**
   \brief special function for "using textures"

   within a texture-garbage-collection run all unused textures get cleaned,
   so this function "uses" the textures. This way they won't get collected
 */

void AGButton::useTextures()
  {
    for(std::map<State,AGBackground>::iterator i=mBG.begin();i!=mBG.end();++i)
      i->second.useTextures();
    for(std::map<State,AGBorder>::iterator i=mBorder.begin();i!=mBorder.end();++i)
      i->second.useTextures();
  }

bool AGButton::isOpaque() const
{
  return true;
}
