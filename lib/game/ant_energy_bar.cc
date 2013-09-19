#include "ant_energy_bar.h"

AntEnergyBar::AntEnergyBar(AGWidget *pParent, const AGRect2 & pRect) :
  AGWidget(pParent,pRect) {
  }

void AntEnergyBar::setHero(AntHero *pHero) {
  mHero=pHero;
}
