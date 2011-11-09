#include <ant_sheep.h>
#include <map.h>
#include <ant_models.h>
#include <ag_mixer.h>
#include <anim_mesh.h>

AntSheep::AntSheep(AntMap* pMap): AntAnimal(pMap)
{
    wasdead=false;
}

AntSheep::~AntSheep() throw()
{

}

void AntSheep::init()
{
    AntEntity::init();
    setMesh(AntModels::createModel(getScene(),"sheep",""));
}


void AntSheep::eventNoJob()
{
    AntEntity::eventNoJob();
    if (dead()) {
        newRestJob(30);
        if (wasdead) {
            getMap()->removeEntity(this);
        }
        wasdead=true;
    }
    if (!giveBirth()) {
        if (rand()%10<5) {
            newMoveJob(0,getTargetPos(),0);
            setMeshState("go");
        } else {
            newRestJob(3);
            setMeshState("eat");
            playSound("sheep");
        }
    }
}
AGVector2 AntSheep::getTargetPos()
{
    AGVector2 p=getPos2D();
    int tries=10;
    // # assure that sheep doesn't walk into water
    while (tries>0) {
        float dx=rand()%100;
        float dy=rand()%100;
        AGVector2 d=AGVector2(dx,dy).normalized()*2;
        AGVector2 t=p+d;
        t=getMap()->truncPos(t);
        tries-=1;
        if (getMap()->getHeight(t.getX(),t.getY())>0) {
            return t;
        }
    }
    return p;
}

AntSheep* AntSheep::createOne()
{
    return new AntSheep(getMap());
}

void AntSheep::setMeshState(const AGString &name)
{
    SceneNode*n=getFirstMesh();
    AnimMesh *mesh=dynamic_cast<AnimMesh*>(n);
    if (mesh)
        mesh->setAnimation(name);
    else
        std::cout<<"no animmesh:"<<(typeid(*n).name())<<std::endl;
}
