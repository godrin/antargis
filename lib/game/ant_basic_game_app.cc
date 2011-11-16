#include "ant_basic_game_app.h"

AntBasicGameApp::AntBasicGameApp(int w, int h): GLApp(w, h)
{
    controlsDisabled=false;
}

void AntBasicGameApp::eventClick(const PickResult& pNodes, int button)
{
    GLApp::eventClick(pNodes, button);

    if (controlsDisabled) {
        cdebug("CONTROLS DISABLED");
        return ;
    }
    if (pNodes.size()>0) {
        PickNode first=pNodes.at(0);
        if (dynamic_cast<TerrainPiece*>(first.node) ||
                dynamic_cast<WaterPiece*>(first.node)) {
            eventMapClicked(first.pos,button);
        } else {
            eventEntitiesClicked(pNodes,button);
        }
    }
}
