#include "ag_layoutfactory.h"
#include "ant_energy_bar.h"
#include "ant_hero_faces.h"

template <class T> class AntGenericCreator : public AGLayoutCreator {
public:
  virtual void create(AGWidget *pParent, const AGRect2 &pRect,
                      const Node &pNode) {
    AGWidget *w = new T(pParent, pRect);
    setResult(w);
  }
};

void initAntargisLayoutCreators() {
  getLayoutFactory()->addCreator("antHeroFaces",
                                 new AntGenericCreator<AntHeroFaces>());
  getLayoutFactory()->addCreator("antEnergy",
                                 new AntGenericCreator<AntEnergyBar>());
}
