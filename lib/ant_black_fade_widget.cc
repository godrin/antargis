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


#include "ant_black_fade_widget.h"

AntBlackFadeWidget::AntBlackFadeWidget(AGWidget *pParent,const AGRect2 &r):
        AGWidget(pParent,r)
{
    alpha=0;
}

AntBlackFadeWidget::AntBlackFadeWidget(const AntBlackFadeWidget& other):
        AGWidget(other) {

}

AntBlackFadeWidget::~AntBlackFadeWidget() throw()
{

}

AntBlackFadeWidget& AntBlackFadeWidget::operator=(const AntBlackFadeWidget& other)
{
    AGWidget::operator=(other);
    return *this;
}



void AntBlackFadeWidget::draw(AGPainter& p)
{
    AGWidget::draw(p);
    p.fillRect(getClientRect(),AGColor(0,0,0,(int)(0xFF*alpha)));

}

void AntBlackFadeWidget::setAlpha(float p)
{
    alpha=p;
}
