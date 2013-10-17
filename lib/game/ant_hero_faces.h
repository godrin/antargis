#ifndef __HERO_FACES_H
#define __HERO_FACES_H

#include "ag_widget.h"
#include "ag_texture.h"
#include <boost/signals.hpp>

class AntPlayer;
class AntHero;

class AntHeroFaces:public AGWidget {
  public:
    AntHeroFaces(AGWidget *pParent, const AGRect2 &pRect);
    virtual ~AntHeroFaces() throw();

    virtual void eventInitEvents();

    void update(AntPlayer *pPlayer);

    boost::signal<void(AntHero*)> sigHeroClicked;
  private:

    bool buttonClicked(AGEvent *e);

    std::vector<AntHero*> heroes;

    AGWidget *aggressionSelector;
};

#endif


