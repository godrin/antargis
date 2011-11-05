#include "ag_frame.h"
#include "ag_screen.h"

#include "rk_debug.h"

AGFrame::AGFrame(AGWidget *pParent,const AGRect2 &pRect,int pWidth,int pWidthH):AGWidget(pParent,pRect),
mWidth(pWidth),mBorder(0),mWidthH(pWidthH<0?pWidth:pWidthH),mTexture((int)width(),(int)height())
{
  mTextureInited=false;
  mUseTexture=true;
  mBg=0;
}
AGFrame::AGFrame(AGWidget *pParent,const AGRect2 &pRect,const AGBorder &pBorder):AGWidget(pParent,pRect),
mWidth(pBorder.getWidth()),mWidthH(pBorder.getWidth()),mBorder(new AGBorder(pBorder)),mTexture((int)width(),(int)height())
{
  mTextureInited=false;
  mUseTexture=true;
  mBg=0;
}

AGFrame::~AGFrame() throw()
  {
    checkedDelete(mBorder);
    checkedDelete(mBg);
  }

AGRect2 AGFrame::getClientRect() const
{
  return getRect().origin().shrink(mWidth,mWidthH);
}

void AGFrame::prepareDraw()
  {
    if(mUseTexture && mBorder)
      {
        //      if(!mTextureInited)
        if(!mTexture.hasTexture())
          {
            AGPainter p(mTexture);
            mBorder->draw(getRect().origin(),p);
            mTextureInited=true;

          }
      }
    AGWidget::prepareDraw();
  }

void AGFrame::draw(AGPainter &p)
  {
    //  CTRACE;
    if(mBg)
      {
        mBg->draw(getRect().origin(),p);
      }
    if(mBorder)
      {
        if(mUseTexture && mTextureInited)
          p.blit(mTexture,mTexture.getRect());
        else
          mBorder->draw(getRect().origin(),p);
      }
  }

void AGFrame::setBackground(const AGBackground &pBg)
  {
    mBg=new AGBackground(pBg);
  }
