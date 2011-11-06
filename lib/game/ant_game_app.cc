#include "ant_game_app.h"
#include "map.h"
#include "ag_layout.h"

AntGameApp::AntGameApp(int w, int h): GLApp(w, h)
{
}

void AntGameApp::init()
{
    layout=new AGLayout(0);
    layout->loadXML(loadFile("data/gui/layout/ant_layout.xml"));
    layout->setApp(this);
    setMainWidget(layout);

    mMap=new AntMap(&getScene(),32,32);
    mMap->loadMap("data/levels/tutorial/tutorial0.antlvl");
}
