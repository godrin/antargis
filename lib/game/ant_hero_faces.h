#ifndef __HERO_FACES_H
#define __HERO_FACES_H

#include "ag_widget.h"
#include "ag_texture.h"

class AntHeroFaces:public AGWidget {
  public:
    AntHeroFaces(AGWidget *pParent, const AGRect2 &pRect);

    void update();
    void setHeroImage(const AGTexture &t);
     

  private:
    AGTexture heroImage;
};

#endif


