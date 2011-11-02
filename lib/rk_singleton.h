/* 
 * File:   rksingleton.h
 * Author: davidkamphausen
 *
 * Created on 16. Februar 2009, 17:47
 */

#ifndef _RKSINGLETON_H
#define	_RKSINGLETON_H

#include <string>
#include <map>
#include <typeinfo>
#include <iostream>

template<class Base>
class RKSingleton;

class RKSingletonBase {
public:
protected:
  RKSingletonBase();

  virtual ~RKSingletonBase() {
  }

  static std::map<std::string, RKSingletonBase*> mSingletons;

  static bool allDestroyed();

private:
  static RKSingletonBase* getInstance(const std::string &pName);
  void registerClass();
  static void destroyAll();

  template<class Base>
  friend class RKSingleton;
  friend void removeEverything();

};

template<class Base>
class RKSingleton : public RKSingletonBase {
public:

  virtual ~RKSingleton() {
  }

  static Base *getInstance() {
    if(allDestroyed())
      return 0;
    std::string name = typeid (Base).name();
    Base *b = dynamic_cast<Base*> (RKSingletonBase::getInstance(name));
    if (b == 0) {
      b = new Base();
      b->registerClass();
    }
    return b;
  }
protected:

  RKSingleton() {
  }

};

#endif	/* _RKSINGLETON_H */

