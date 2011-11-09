#include <ant_animal.h>
#include <ant_entity_selector.h>

AntAnimal::AntAnimal(AntMap *pMap):AntEntity(pMap),lastBirth(rand()%40)
{
    mDead=false;
}

bool AntAnimal::giveBirth()
{
    lastBirth+=1;
    // BIRTHRATE is here:
    if (lastBirth>40) {
        AntMap::EntityList fishList=getMap()->getSelectedEntities(std::auto_ptr<AntEntitySelector>(new AntEntityClassSelector<AntAnimal>()));
        if (fishList.size()>50)
            return false;
        // make child
        AntAnimal *fish=createOne();
        fish->setPos(getPos2D());
        getMap()->insertEntity(fish);
        getMap()->mapChanged();
        newRestJob(2);
        lastBirth=-rand()%10;
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
