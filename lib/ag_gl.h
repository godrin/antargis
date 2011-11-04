#ifndef AG_GL_H
#define AG_GL_H

#include <GLee.h>

#ifdef __APPLE__
// patch for mac os
#ifdef GL_GLEXT_VERSION
#undef GL_GLEXT_VERSION
#endif
#endif

#include <SDL_opengl.h>
#include <GL/glu.h>

#endif
