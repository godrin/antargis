/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_table.cc
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

#include "ag_table.h"
#include "rk_debug.h"

AGTable::AGTable(AGWidget *pWidget,const AGRect2 &pRect):
  AGWidget(pWidget,pRect),
  w(0),h(0),xw(0),yw(0),mInserted(false),mRoundPositions(true)
  {
  }

AGTable::~AGTable() throw()
  {
  }

/// add a column with fixed size of 'size'
/// @param size gives to size of the column in pixels
void AGTable::addFixedColumn(float size)
  {
    if(mInserted)
      return;
    cols.push_back(std::make_pair(size,true));
    w++;
  }
/// add a row with fixed size of 'size'
/// @param size gives to size of the row in pixels
void AGTable::addFixedRow(float size)
  {
    if(mInserted)
      return;
    rows.push_back(std::make_pair(size,true));
    h++;
  }

/// add a column with relative size.
/// @param weight
///  All weights of columns are summed up. After giving their space
///  to the fixed columns ( AGTable::addFixedColumn() ), each relative sized
///  column gets its relative size according the weight. (size_left*weight/sum(weights))
void AGTable::addColumn(float weight)
  {
    if(mInserted || weight<=0.0f)
      return;
    cols.push_back(std::make_pair(weight,false));
    w++;
    xw+=weight;
  }

/// add a row with relative size.
/// @param weight
///  All weights of rows are summed up. After giving their space
///  to the fixed rows ( AGTable::addFixedRow() ), each relative sized
///  row gets its relative size according the weight. (size_left*weight/sum(weights))
void AGTable::addRow(float weight)
  {
    if(mInserted || weight<=0.0f)
      return;
    rows.push_back(std::make_pair(weight,false));
    h++;
    yw+=weight;
  }

/// adds a widget to the given cell (px,py)
void AGTable::addChild(int px,int py,AGWidget *pWidget)
  {
    if(px>=w || py>=h || px<0 || py<0)
      {
        std::cerr<<"ERROR: wrong table-position:"<<px<<","<<py<<std::endl;
        std::cerr<<"table size is:"<<w<<","<<h<<std::endl;
        throw std::runtime_error("WARNING:wrong input position");
        return;
      }

    if(!mInserted)
      children=std::vector<AGWidget*>(w*h,(AGWidget*)0);

      if(children[px+py*w])
        cdebug("Children "<<px<<","<<py<<" already set!");

      mInserted=true;
      children[px+py*w]=pWidget;
      AGWidget::addChild(pWidget);
  }

/// returns the Position and size of a given cell (x,y)
AGRect2 AGTable::getClientRect(int x,int y) const
{
  AGRect2 mrect(0,0,0,0);

  int mx,my;

  // first get the fixed sizes
  int fx,fy;
  int mfx,mfy; // max
  fx=fy=0;
  mfx=mfy=0;

  // in y dir
  for(mx=0;mx<w;mx++)
    {
      fy=0;
      //      if(cols[mx]==0.0)
      for(my=0;my<h;my++)
        {
          if(rows[my].second)
            fy+=(int)(rows[my].first);
        }
      mfy=std::max(mfy,fy);
    }

  // x dir
  for(my=0;my<h;my++)
    {
      fx=0;
      //      if(rows[my]==0.0) // only check fixed
      for(mx=0;mx<w;mx++)
        {
          if(cols[mx].second)
            {
              fx+=(int)(cols[mx].first);
            }

        }
      mfx=std::max(mfx,fx);
    }


  if(cols[x].second)
    mrect.setWidth(cols[x].first);
  else
    mrect.setWidth((width()-mfx)*cols[x].first/xw);

  if(rows[y].second)
    mrect.setHeight(rows[y].first);
  else
    mrect.setHeight((height()-mfy)*rows[y].first/yw);

  int ax=0;
  for(mx=0;mx<w;mx++)
    {
      if(mx==x)
        {
          mrect.setX(ax);
          break;
        }
      else
        {
          if(cols[mx].second)
            ax+=(int)(cols[mx].first);
          else
            ax+=(int)((width()-mfx)*cols[mx].first/xw);
        }
    }

  int ay=0;
  for(my=0;my<h;my++)
    {
      if(my==y)
        {
          mrect.setY(ay);
          break;
        }
      else
        {
          if(rows[my].second)
            ay+=(int)(rows[my].first);
          else
            ay+=(int)((height()-mfy)*rows[my].first/yw);
        }
    }
  return mrect;
}

void AGTable::arrange()
  {
    CTRACE;
    int mx,my;

    // first get the fixed sizes
    int fx,fy;
    int mfx,mfy; // max
    fx=fy=0;
    mfx=mfy=0;

    // in y dir
    for(mx=0;mx<w;mx++)
      {
        fy=0;
        //      if(cols[mx]==0.0)
        for(my=0;my<h;my++)
          {
            if(rows[my].second)
              fy+=(int)(rows[my].first);
          }
        mfy=std::max(mfy,fy);
      }

    // x dir
    for(my=0;my<h;my++)
      {
        fx=0;
        //      if(rows[my]==0.0) // only check fixed
        for(mx=0;mx<w;mx++)
          {
            if(cols[mx].second)
              {
                fx+=(int)(cols[mx].first);
              }
          }
        mfx=std::max(mfx,fx);
      }

    // TODO: assign width's and heights for all fixed !!!
    for(mx=0;mx<w;mx++)
      for(my=0;my<h;my++)
        {
          if(cols[mx].second)
            {
              AGWidget *wd=children[mx+my*w];
              if(wd)
                {
                  wd->setWidth(cols[mx].first);
                }
            }
          if(rows[my].second)
            {
              AGWidget *wd=children[mx+my*w];
              if(wd)
                {
                  wd->setHeight(rows[my].first);
                }
            }
        }

    // assign width's and height's for all non-fixed
    for(mx=0;mx<w;mx++)
      for(my=0;my<h;my++)
        {
          if(!cols[mx].second)
            {
              AGWidget *wd=children[mx+my*w];
              if(wd)
                {
                  wd->setWidth((int)((width()-mfx)*cols[mx].first/xw));
                }
            }
          if(!rows[my].second)
            {
              AGWidget *wd=children[mx+my*w];
              if(wd)
                {
                  wd->setHeight((int)((height()-mfy)*rows[my].first/yw));
                }
            }
        }

    // assign positions
    // first x
    float ax=0;
    for(mx=0;mx<w;mx++)
      {
        float maxx=0;
        for(my=0;my<h;my++)
          {
            AGWidget *wd=children[mx+my*w];
            if(wd)
              {
                wd->setLeft(ax);

                maxx=std::max(maxx,wd->width());
              }
          }
        ax+=maxx;
      }

    // now y
    float ay=0;
    for(my=0;my<h;my++)
      {
        float maxy=0;
        for(mx=0;mx<w;mx++)
          {
            AGWidget *wd=children[mx+my*w];
            if(wd)
              {
                wd->setTop(ay);

                maxy=std::max(maxy,wd->height());
              }
          }
        ay+=maxy;
      }

  }


void AGTable::setWidth(float w)
  {
    AGWidget::setWidth(w);
    arrange();
  }
void AGTable::setHeight(float w)
  {
    AGWidget::setHeight(w);
    arrange();
  }

size_t AGTable::getRows() const
{
  return rows.size();
}
size_t AGTable::getColumns() const
{
  return cols.size();
}

float AGTable::getColumn(size_t c) const
{
  assert(c<cols.size());
  return cols[c].first;
}
float AGTable::getRow(size_t c) const
{
  assert(c<rows.size());
  return rows[c].first;
}


void AGTable::modifyColumn(size_t index,float w)
  {
    if(cols.size()>index)
      {
        cols[index].first=w;
      }
    arrange();
  }
void AGTable::modifyRow(size_t index,float w)
  {
    CTRACE;
    if(rows.size()>index)
      {
        rows[index].first=w;
      }
    arrange();
  }
