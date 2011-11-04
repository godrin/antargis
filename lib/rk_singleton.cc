/* 
 * File:   rksingleton.cc
 * Author: davidkamphausen
 * 
 * Created on 16. Februar 2009, 17:47
 */

#include "rk_singleton.h"
#include <set>
#include <typeinfo>
#include <cstdlib>

std::map<std::string, RKSingletonBase*> RKSingletonBase::mSingletons;

static bool firstUse = true;
static bool everythingDestroyed=false;

void removeEverything();

RKSingletonBase::RKSingletonBase() {
  if (firstUse) {
    firstUse = false;
    atexit(&removeEverything);
  }
}


bool RKSingletonBase::allDestroyed()
{
  return everythingDestroyed;
}

void RKSingletonBase::destroyAll()
{
  std::cout<<"destroyAll"<<std::endl;
  everythingDestroyed=true;
  for(std::map<std::string, RKSingletonBase*>::iterator i = mSingletons.begin();i!=mSingletons.end();i++)
    delete i->second;
}

void RKSingletonBase::registerClass() {
  if(everythingDestroyed)
    return;
  std::string name(typeid (*this).name());
  mSingletons.insert(std::make_pair(name, this));
}

RKSingletonBase* RKSingletonBase::getInstance(const std::string &pName) {
  if(everythingDestroyed)
    return 0;

  std::map<std::string, RKSingletonBase*>::iterator i = mSingletons.find(pName);

  if (i == mSingletons.end()) {
    return NULL;
  }
  return i->second;

}

void removeEverything() {
  std::cout << "RMOVE ALL" << std::endl;
  RKSingletonBase::destroyAll();
}



/*
template<class Base>
RKSingleton<Base>::RKSingleton() {
}

template<class Base>
RKSingleton<Base>::~RKSingleton() {
}

template<class Base>
static Base *RKSingleton<Base>::getInstance() {
    std::string name=typeid(Base).name();
    Base *b=dynamic_cast<Base*>(getInstance(name));
    if(b==0)
        b=new Base();
    return b;
}
 */