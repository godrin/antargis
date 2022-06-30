#include "ag_layoutfactory.h"
#include "ant_energy_bar.h"
#include "ant_hero_faces.h"

template <class T> class AntGenericCreator : public AGLayoutCreator {
public:
  virtual AGLayoutCreationResult create(AGWidget *pParent, const AGRect2 &pRect,
                      const Node &pNode) {
    return AGLayoutCreationResult(new T(pParent, pRect));
  }
};

void initAntargisLayoutCreators() {
  getLayoutFactory()->addCreator("antHeroFaces",
                                 new AntGenericCreator<AntHeroFaces>());
  getLayoutFactory()->addCreator("antEnergy",
                                 new AntGenericCreator<AntEnergyBar>());
}
