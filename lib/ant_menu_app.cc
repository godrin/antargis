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


#include "ant_menu_app.h"
#include "ag_layout.h"

AntMenuApp::AntMenuApp():AGApplication()
{

}

AntMenuApp::~AntMenuApp() throw()
{

}


void AntMenuApp::init()
{
  layout=new AGLayout(0);
  layout->loadXML(loadFile("data/gui/layout/mainmenu.xml"));
  setMainWidget(layout);
  
  layout->getChild("quit")->sigClick.connect(slot(this,&AntMenuApp::eventQuitClicked));
}

bool AntMenuApp::eventQuitClicked(AGEvent* pEvent)
{
tryQuit();
return false;
}



bool AntMenuApp::eventFrame(float pTime)
{
    SDL_Delay(10);
    return AGApplication::eventFrame(pTime);
}

