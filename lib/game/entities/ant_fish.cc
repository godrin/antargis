#include <ant_fish.h>
#include <ant_models.h>
#include <map.h>
#include <ant_field.h>
#include <ag_main.h>

AntFish::AntFish(AntMap* pMap): AntAnimal(pMap)
{

}
void AntFish::init()
{
  AntAnimal::init();
  setProvide("fish",true);
  setSpeed(0.6);

  setOnGround(false);
  setPos(AGVector3(0,0,-0.3));

  setupMesh();

  resource.set("food",1);
}

void AntFish::setupMesh()
{
  setMesh("fish");
}

void AntFish::moveAround() {
  setPos(AGVector3(getPos2D(),-0.3));
  newMoveJob(0,getTargetPos(),0);
  //setMesh("fish","go");
  setMesh("fish");
}

AGVector2 AntFish::getTargetPos() {
  auto p=getPos2D();
  int tries=30;
  float currentHeight=getMap()->getHeight(p.getX(),p.getY());
  // assure that fish doesn't walk onto land
  while(tries>0) {
    auto d=AGVector2(RANDOM(1.0f)-0.5,RANDOM(1.0f)-0.5).normalized()*2;
    auto t=p+d;
    t=getMap()->truncPos(t);
    tries-=1;
    float height=getMap()->getHeight(t.getX(),t.getY());
    if(height<=-0.5 || height<=currentHeight) {
      return t;
    }
  } 
  return p;
}

AntFish *AntFish::createOne()
{
  return new AntFish(getMap());
}
