#ifndef AG_VDEBUG
#define AG_VDEBUG

#include <rk_debug.h>

AGEXPORT void agAssertGL(std::string s);
AGEXPORT void checkGLReal(const std::string &pString);

#define assertGL agAssertGL(::toString(__FILE__)+::toString(" ")+::toString(__LINE__)+::toString(" ")+::toString( __PRETTY_FUNCTION__))

#define checkGL(x) checkGLReal(::toString(__FILE__)+::toString(" ")+::toString(__LINE__)+::toString(" ")+::toString( __PRETTY_FUNCTION__)+x)

#endif
