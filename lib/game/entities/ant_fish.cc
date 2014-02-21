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
  AntEntity::init();
  setProvide("fish",true);
  setSpeed(0.6);
  mFoodAdd=0;

  setOnGround(false);
  setPos(AGVector3(0,0,-0.3));

  setupMesh();

  resource.set("food",1);
  alreadyDead=false;
}

void AntFish::setupMesh()
{
  setMesh("fish");
}

void AntFish::eventNoJob() {
  AntAnimal::eventNoJob();

  if(dead()) {
    newRestJob(30);
    if(alreadyDead)
      getMap()->removeEntity(this);

    alreadyDead=true;

      return;
  }

  if (!giveBirth()) {
    setPos(AGVector3(getPos2D(),-0.3));
    newMoveJob(0,getTargetPos(),0);
    setMesh("fish","go");
  }

  mFoodAdd++;
//#puts "FOOOOOD #{@foodAdd}"
    if(mFoodAdd>3 && resource.get("food")<10) {
      resource.add("food",1);
        mFoodAdd=0;
    }

}

AGVector2 AntFish::getTargetPos() {
  auto p=getPos2D();
  int tries=10;
  // assure that sheep doesn't walk into water
  while(tries>0) {
    auto d=AGVector2(RANDOM(1)-0.5,RANDOM(1)-0.5).normalized()*2;
    auto t=p+d;
    t=getMap()->truncPos(t);
    tries-=1;
    if(getMap()->getHeight(t.getX(),t.getY())<0) {
      return t;
    }
  } 
  return p;
}

AntFish *AntFish::createOne()
{
  return new AntFish(getMap());
}
