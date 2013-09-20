#include "ant_hero_faces.h"
#include "ag_image.h"
#include "ant_player.h"
#include "ant_hero.h"
#include "ant_energy_bar.h"


AntHeroFaces::AntHeroFaces(AGWidget *pParent, const AGRect2 &pRect):
  AGWidget(pParent,pRect) {
  }

void AntHeroFaces::update(AntPlayer *pPlayer) {
  int index=0;
  for(AntHero* hero: pPlayer->getHeroes()) {

    AGString widgetName="HeroImage";
    widgetName+=AGString(index);
    std::cout<<"NAME:"<<widgetName<<std::endl;
    AGWidget *widget=getChild(widgetName);

    AGImage *image=dynamic_cast<AGImage*>(widget);
    if(image) {
      image->setTexture(hero->getImage());
    }
    widget=getChild(AGString("HeroBar")+AGString(index));
    AntEnergyBar *bar=dynamic_cast<AntEnergyBar*>(widget);
    if(bar) {
      bar->setHero(hero);
    }

    index++;
  }
  for(index=0;index<6;index++) {
    AGString widgetName="hero";
    widgetName+=AGString(index);
    AGWidget *widget=getChild(widgetName);
    widget->setVisible(index<pPlayer->getHeroes().size());
  }
}


