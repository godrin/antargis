#include "ant_energy_bar.h"
#include "ant_hero.h"

std::vector<AGString> AntEnergyBar::dataKeys={"energy","morale","food","troops","exp"};
std::map<AGString,AGColor> AntEnergyBar::colors={
  {"troops",AGColor(0xcf,0,0)},
  {"energy",AGColor(0,0xcf,0)},
  {"morale",AGColor("#0000FF")},
  {"morale_defeated",AGColor("#5555ff")},
  {"food",AGColor("#975500")},
  {"exp",AGColor("#FFFFFF")}};
AntEnergyBar::AntEnergyBar(AGWidget *pParent, const AGRect2 & pRect) :
  AGWidget(pParent,pRect) {
    vborder=hborder=0;
    hborder=4;
    mHero=0;
  }

void AntEnergyBar::setHero(AntHero *pHero) {
  mHero=pHero;
}

void AntEnergyBar::update() {
}

float AntEnergyBar::check(float f) {
  return std::min(std::max(0.0f,f),1.0f);
}
void AntEnergyBar::updateValues() {
  values.clear();
  if(mHero) {
    values["energy"]=mHero->getEnergy();
    values["morale"]=mHero->getMorale();
    values["troops"]=mHero->getMenWithoutBoss().size()/60.0;
    values["food"]=mHero->getFood();
    values["exp"]=mHero->experience;
  }
}
void AntEnergyBar::draw(AGPainter &p) {
  CTRACE;
  AGWidget::draw(p);

  updateValues();

  if(values.size()==0)
    return;

  auto r=p.getRect().origin();
  auto w=r.width();
  auto h=r.height()-vborder;

  float v=vborder/2;

  auto mh=h/(values.size()*3);

  auto i=0;
  AGColor c;
  for(auto k:dataKeys) {
    std::cout<<"VVVV for "<<k<<":"<<values[k]<<std::endl;
    v=check(values[k]);
    if(k=="morale" && mHero->defeated()) 
      c=colors["morale_defeated"];
    else
      c=colors[k];

    auto a0=c*0.7;
    auto a1=c*0.7;
    auto a2=c*1.0;
    auto a3=c*0.8;

    std::cout<<"MR:"<<hborder<<" p:"<<i*mh<<" i:"<<i<<" mh:"<<mh<<" "<<w<< " v:"<<v<<std::endl;

    auto mr=AGRect2(hborder,i*mh,(w-hborder*2)*v,mh*2);

    std::cout<<"MR:"<<mr<<std::endl;

    p.drawGradient(mr,a0,a1,a2,a3);
    i+=3;
  }
}
