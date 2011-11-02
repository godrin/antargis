#include "rk_base.h"
#include "rk_rubyobj.h"
#include <assert.h>
#include <typeinfo>
#include <set>
#include <map>
#include <iostream>
#include "rk_logging.h"


#ifdef __WIN32__
#include <winsock2.h> // fix problem with fd_set
#endif
#include <ruby.h>

bool mRubyQuitting = false;
bool mRubyObjectsExistant = false;

void deleteCppReference(AGRubyObject *cObject);


class RubyObjects : public std::set<AGRubyObject*> {
public:

  RubyObjects() {
    mRubyObjectsExistant = true;
  }

  virtual ~RubyObjects() {
    mRubyObjectsExistant = false;
  }
};

// this set keeps track of all valid instances of AGRubyObject
RubyObjects mRubyObjects;
std::set<AGRubyObject*> mRemovedRubyObjects;

void setQuitting() {
  mRubyQuitting = true;
}

// swig-function for handling "tracked" objects
// BEWARE: this does not support multiple libraries yet (FIXME)
VALUE convertCpp2Ruby(AGRubyObject *cObject);

AGEXPORT void *getAddressOfRubyObject(AGRubyObject *o) {
  return o;
}

AGEXPORT bool rubyObjectExists(void *po) {
  VALUE v = convertCpp2Ruby((AGRubyObject*) po);
  return (v != Qnil);
}

AGRubyObject::AGRubyObject() throw () {
  logger::gc << "new AGRubyObject:" << this << logger::endl;
  if(mRubyObjects.find(this) != mRubyObjects.end())
    logger::err<< "RubyObject does already exist ????"<<logger::endl;
  // assert(mRubyObjects.find(this) == mRubyObjects.end());
  mRubyObjects.insert(this);
  size_t oSize = mRemovedRubyObjects.size();

  mRemovedRubyObjects.erase(this);
  if (oSize != mRemovedRubyObjects.size())
    logger::gc<< "Collision - removed rubyobject's address is overwritten!" << logger::endl;
  logger::gc << "current ruby#:" << mRubyObjects.size() << " removed:" << mRemovedRubyObjects.size() << logger::endl;
}

AGRubyObject::~AGRubyObject() throw () {
  logger::gc << "removed AGRubyObject:" << this << logger::endl;
  assert(mRubyObjects.find(this) != mRubyObjects.end());
  assert(mRemovedRubyObjects.find(this) == mRemovedRubyObjects.end());
  //std::cerr<<"AGRubyObject::Removed:"<<this<<std::endl;
  for (std::set<AGBaseObject*>::iterator i = mReferences.begin(); i != mReferences.end(); i++)
    (*i)->baseClear();
  mRubyObjects.erase(this);
  mRemovedRubyObjects.insert(this);
  //deleteCppReference(this);
}


/// override this function to mark your children

void AGRubyObject::mark() throw () {
}


// call this function with any object you want to mark.
// recursive should be set true only in one direction, otherwise you'll generate endless-loops (!!)

void AGRubyObject::markObject(AGRubyObject *o, bool recursive) throw () {
  //std::cerr<<"markObject:"<<o<<std::endl;
  // o must be a valid ruby-object
  assert(mRubyObjects.find(o) != mRubyObjects.end());

  logger::gc << "AGRubyObject " << o << " marked by "<<this<<logger::endl;

  // look up, if it's registered within ruby
  VALUE v = convertCpp2Ruby(o);
  if (v != Qnil) {
    assert(DATA_PTR(v) == o);
    //std::cout<<"V:"<<v<<std::endl;
    // then mark it
    rb_gc_mark(v);

  }

  assert(o);
  // recurse through hierarchy
  if (recursive)
    o->mark(); // call this directly
}

void AGRubyObject::clear() throw () {
}

void AGRubyObject::addRef(AGBaseObject *o) {
  mReferences.insert(o);
}

void AGRubyObject::deleteRef(AGBaseObject *o) {
  mReferences.erase(o);
}



/// this is the marking function, that gets called by ruby
/// it handles all the AGRubyObjects

void general_markfunc(void *ptr) {
  if (!ptr) {
    logger::err << ("WARNING: a ptr==0 was given in general_markfunc!") << logger::endl;
    return; // ignore this !
  }
  assert(ptr);
  // the given object must be a AGRubyObject and it must be valid (it's in mRubyObjects)
  AGRubyObject *o = static_cast<AGRubyObject*> (ptr);
  if (mRubyObjects.find(o) == mRubyObjects.end())
    {
      logger::err << "OLD RUBYOBJ:" << (mRemovedRubyObjects.find(o) != mRemovedRubyObjects.end()) << ":" << o << logger::endl;

      VALUE v=convertCpp2Ruby(o);
      rb_gv_set("antargis_buggy_object",v);
      //rb_eval_string("puts $antargis_buggy_object");
      return;
      rb_raise(rb_eFatal, "A C++ object is still under surveilance of ruby, but is already deleted!");
    }
  assert(mRubyObjects.find(o) != mRubyObjects.end());

#ifdef GCDEBUG
  printf("mark: 0x%lx\n", o->mRUBY);
#endif

  assert(o);


  logger::gc << "mark called for "<<o<< "("<<typeid(*o).name()<<logger::endl;
  o->mark();
}

IsRubyObject isRubyObject(void *o) {
  AGRubyObject *ro = (AGRubyObject*) o;

  if (!mRubyObjectsExistant) {
    logger::err << "Could not check, because rubyObjects was discarded" << logger::endl;
    return RUBY_OBJECT_UNKNOWN;
  }

  if (mRubyObjects.find(ro) == mRubyObjects.end())
    return RUBY_OBJECT_NO;
  else
    return RUBY_OBJECT_YES;

}

/**
   saveDelete is used to delete AGRubyObjects savely.
 */
bool saveDelete(AGRubyObject *o) {
  if (mRubyQuitting)
    return false; // we are quitting - so memory is discarded anyway - hopefully ;-)

  // check, if this object is existant any longer
  // in case we're quitting this the deletion order is not defined for ruby-objects !!
  if (mRubyObjects.find(o) == mRubyObjects.end()) {
#ifdef GC_DEBUG
    std::cerr << "RubyObject " << o << " no longer existant - maybe we're quitting ?!" << std::endl;
#endif
    return false;
  }

  assert(o);
  // send object a message, that it will be deleted. This can help with detachin connections
  // between objects.
  o->clear();

  VALUE v = convertCpp2Ruby(o);
  if (v != Qnil)
    return false; // do not delete - it's under ruby's control!


  delete o;
  return true;
}

AGBaseObject::AGBaseObject(AGRubyObject *p) throw ()
: mp(p) {
  if (p)
    p->addRef(this);
}

AGBaseObject::~AGBaseObject() throw () {
  if (mp)
    mp->deleteRef(this);
}

void AGBaseObject::baseClear() throw () {
  mp = 0;
}
