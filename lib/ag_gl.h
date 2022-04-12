#ifndef AG_GL_H
#define AG_GL_H

///#include <GLee.h>

#ifdef __APPLE__
// patch for mac os
#ifdef GL_GLEXT_VERSION
#undef GL_GLEXT_VERSION
#endif
#endif
#define NO_SDL_GLEXT
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL_opengl.h>

#endif
