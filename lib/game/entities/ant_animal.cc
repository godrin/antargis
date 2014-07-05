#include <ant_animal.h>
#include <ant_entity_selector.h>

AntAnimal::AntAnimal(AntMap *pMap):AntEntity(pMap),mLastBirth(rand()%40)
{
  mDead=false;
  mAlreadyDead=false;
}

void AntAnimal::init() {
  AntEntity::init();
  mFood=0;
}

void AntAnimal::eventNoJob() {
  AntEntity::eventNoJob();

  if(dead()) {
    newRestJob(30);
    if(mAlreadyDead)
      getMap()->removeEntity(this);

    mAlreadyDead=true;

    return;
  }

  if (!giveBirth()) {
    moveAround();
  }

  mFood++;
  if(mFood>3 && resource.get("food")<10) {
    resource.add("food",1);
    mFood=0;
  }

}

void AntAnimal::moveAround() {
}

bool AntAnimal::giveBirth()
{
  mLastBirth+=1;
  // BIRTHRATE is here:
  if (mLastBirth>40) {
    AntMap::EntityList fishList=getMap()->getSelectedEntities(std::auto_ptr<AntEntitySelector>(new AntEntityClassSelector<AntAnimal>()));
    if (fishList.size()>50)
      return false;
    // make child
    AntAnimal *fish=createOne();
    fish->init();
    fish->setPos(getPos2D());
    getMap()->insertEntity(fish);
    getMap()->mapChanged();
    newRestJob(2);
    mLastBirth=-rand()%10;
    return true;
  }
  return false;

}

bool AntAnimal::dead()
{
  return mDead;
}

void AntAnimal::die()
{
  mDead=true;
}
