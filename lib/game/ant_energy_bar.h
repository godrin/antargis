#ifndef __ANT_ENERGY_BAR_H
#define __ANT_ENERGY_BAR_H

#include "ag_widget.h"

#include <boost/weak_ptr.hpp>
#include <map>

class AntHero;

class AntEnergyBar: public AGWidget {
  public:
    AntEnergyBar(AGWidget *pParent, const AGRect2 & pRect);

    void setHero(AntHero *pHero);

    void update();
  private:
    float check(float f);
    void draw(AGPainter &p);
    void updateValues();

    // FIXME: use shared_ptr or weak_ptr
    AntHero *mHero;
    int vborder,hborder;
    static std::vector<AGString> dataKeys;
    static std::map<AGString,AGColor> colors;
    std::map<AGString,float> values;
};

#endif

