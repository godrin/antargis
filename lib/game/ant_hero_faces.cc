#include "ant_hero_faces.h"
#include "ag_image.h"


AntHeroFaces::AntHeroFaces(AGWidget *pParent, const AGRect2 &pRect):
  AGWidget(pParent,pRect) {
  }



void AntHeroFaces::update() {
  AGWidget *widget=getChild("HeroImage0");
  AGImage *image=dynamic_cast<AGImage*>(widget);
  if(image) {
    image->setTexture(heroImage);
  }
}


void AntHeroFaces::setHeroImage(const AGTexture &t) {
  heroImage=t;
}
