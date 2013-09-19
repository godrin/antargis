#ifndef __ANT_ENERGY_BAR_H
#define __ANT_ENERGY_BAR_H

#include "ag_widget.h"

#include <boost/weak_ptr.hpp>

class AntHero;

class AntEnergyBar: public AGWidget {
  public:
    AntEnergyBar(AGWidget *pParent, const AGRect2 & pRect);

    void setHero(AntHero *pHero);

    void update();
  private:
    // FIXME: use shared_ptr or weak_ptr
    AntHero *mHero;
};

#endif

