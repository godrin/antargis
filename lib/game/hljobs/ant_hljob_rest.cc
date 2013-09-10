#include "ant_hljob_rest.h"
#include "ant_hero.h"
#include "ant_man.h"
#include "ant_formation_rest.h"

#include <algorithm>
#include <cmath>

AntHLJobRest::AntHLJobRest(AntBoss* pBoss, float pTime): AntHLJob(pBoss),mTime(pTime)
{
    firstTime=true;
    spreadingThings=false;
    jobFinished=false;
    getBoss()->setFormation(new AntFormationRest(getBoss()));
}

void AntHLJobRest::check(AntHero* man)
{
    if (firstTime) {
        man->newRestJob(mTime);
        man->setFire(true);
        firstTime=false;
    } else {
        jobFinished=true;
//     man->newRestJob(1000);
    }
    eat(man);
}


void AntHLJobRest::check(AntMan* man)
{
    switch (man->getMode()) {
    case AntMan::REST_EAT: {
        spreadThings();
        eat(man);
        sit(man);
        man->setMode(AntMan::REST_SIT);
        break;
    }
    case AntMan::REST_SIT: {
        sit(man);
        if (man->getFood()<0.5 && heroHasFood()) {
            man->newMoveJob(0,getBossEntity()->getPos2D(),0);
        }
        break;
    }
    default:
        man->newMoveJob(0,getBossEntity()->getPos2D(),0);
        man->setMode(AntMan::REST_EAT);

    }
}

bool AntHLJobRest::heroHasFood()
{
    return getBossEntity()->resource.get("food")>0;
}

void AntHLJobRest::eat(AntPerson* man)
{
    if (man->getFood()<0.5) {
        if (man->resource.get("food")>0) {
            man->incFood(1);
            man->resource.sub("food",1);
        } else if (heroHasFood()) {
            man->incFood(1);
            getBoss()->getEntity()->resource.sub("food",1);
        }
    }

}


void AntHLJobRest::spreadThings()
{
    if (!spreadingThings) {
        spreadingThings=true;

        spreadThing("sword");
        spreadThing("shield");
        spreadThing("bow");
        spreadThing("boat");

    }

    //FIXME: spread food
    spreadFood();

}
void AntHLJobRest::spreadFood()
{
    float food=0;

    std::vector< AntPerson* > men=getMenWithBoss();

    for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
        food+=(*i)->resource.get("food");
    }

    int min=(int)(food/men.size());

    for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
        (*i)->resource.set("food",min);
    }
    // spread rest on first
    food-=min*men.size();
    for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
        if (food<0)
            break;
        (*i)->resource.add("food",1);
        food-=1;
    }

}

class CompareResourceDesc {
public:
    CompareResourceDesc(const AGString &pwhat):what(pwhat) {}
    bool operator()(AntEntity *a,AntEntity *b) {
        return a->resource.get(what)<b->resource.get(what);
    }
private:
    AGString what;
};

void AntHLJobRest::spreadThing(const AGString& what)
{
    std::vector< AntPerson* > men=getMenWithoutBoss();
    float count=0;

    for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
        count+=(*i)->resource.get(what);
    }
    count+=getBossEntity()->resource.get(what);

    if (count>men.size()) {
        // give everyone 1 of these and put the rest on the hero
        for (std::vector<AntPerson*>::iterator i=men.begin();i!=men.end();i++) {
            (*i)->resource.set(what,1);
        }
        getBossEntity()->resource.set(what,count-men.size());
    } else {
        // now give to those who had a weapon and to hero (at first)
        std::vector<AntEntity*> men2;
        men2.push_back(getBossEntity());
        std::copy(men.begin(),men.end(),std::back_inserter(men2));
        std::sort(men2.begin(),men2.end(),CompareResourceDesc(what));
        for (std::vector<AntEntity*>::iterator i=men2.begin();i!=men2.end();i++) {
            (*i)->resource.set(what,count>0?1:0);
            count-=1;
        }

    }
}


AGVector2 AntHLJobRest::basePos()
{
    return getBossEntity()->getPos2D();
}

bool AntHLJobRest::finished()
{
    return jobFinished;
}
