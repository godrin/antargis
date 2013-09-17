#include "ag_layoutfactory.h"
#include "ant_hero_faces.h"

class AntHeroFacesCreator:public AGLayoutCreator
{
public:
  virtual void create(AGWidget *pParent,const AGRect2 &pRect,const Node &pNode) {
    AGWidget *w=new AntHeroFaces(pParent,pRect);
    setResult(w);
  }
};

void initAntargisLayoutCreators() {
  getLayoutFactory()->addCreator("antHeroFaces",new AntHeroFacesCreator);
}
