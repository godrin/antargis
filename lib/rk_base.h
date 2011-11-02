/**
 * This file provides a define "AGEXPORT" to be used for exporting functions and classes
 * out of DLLs on Windows.
 * For this to function you'll just insert it:
 * 1) before a function declaration:
 *    AGEXPORT void myFunction();
 * 2) or inbetween a class-definition:
 *    class AGEXPORT MyClass {
 *    .....
 *    };
 * No additional configuration should be needed.
 * including within swig is taken care of. 
 *  
 * */

#ifndef AG_BASE_H
#define AG_BASE_H

#ifdef WIN32
#ifdef AGIMPORT
#define AGEXPORT __declspec(dllimport)
#else
#undef AGEXPORT
#define AGEXPORT __declspec(dllexport)
#endif // AGIMPORT
#else

#undef AGEXPORT
#define AGEXPORT

#endif // WIN32

#ifdef SWIG
#undef AGEXPORT
#define AGEXPORT
#endif

#include <iostream>

class AGRubyObject;

enum IsRubyObject {
  RUBY_OBJECT_YES, RUBY_OBJECT_NO, RUBY_OBJECT_UNKNOWN
};

AGEXPORT IsRubyObject isRubyObject(void *o);
AGEXPORT bool saveDelete(AGRubyObject *o);


/** checkedDelete deletes o, but checks before if o is a AGRubyObject
 **/
template<class T>
bool checkedDelete(T *o) {
  IsRubyObject iro = isRubyObject((void*) o);
  if (iro == RUBY_OBJECT_YES) {
    std::cerr << "Possible error in checkedDelete(.)" << std::endl;
    return false;
  } else if (iro == RUBY_OBJECT_NO) {
    delete o;
    return true;
  } else {
    std::cerr << "Could not check, because rubyObjects was discarded" << std::endl;
    return false;
  }
}

/** checkedDelete deletes o, but checks before if o is a AGRubyObject
 **/
template<class T>
bool checkedDeleteArray(T *o) {
  IsRubyObject iro = isRubyObject((void*) o);
  if (iro == RUBY_OBJECT_YES) {
    std::cerr << "Possible error in checkedDelete(.)" << std::endl;
    return false;
  } else if (iro == RUBY_OBJECT_NO) {
    delete[] o;
    return true;
  } else {
    std::cerr << "Could not check, because rubyObjects was discarded" << std::endl;
    return false;
  }
}

#endif
