/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ANT_BLACK_FADE_WIDGET_H
#define ANT_BLACK_FADE_WIDGET_H

#include "ag_widget.h"

class AntBlackFadeWidget:public AGWidget
{

public:
    AntBlackFadeWidget(AGWidget *pParent,const AGRect2 &r);
//    AntBlackFadeWidget(const AntBlackFadeWidget& other);
    virtual ~AntBlackFadeWidget() throw();
    //virtual AntBlackFadeWidget& operator=(const AntBlackFadeWidget& other);
    
    void draw(AGPainter &p);
    
    void setAlpha(float p);
private:
  float alpha;
};

#endif // ANT_BLACK_FADE_WIDGET_H
