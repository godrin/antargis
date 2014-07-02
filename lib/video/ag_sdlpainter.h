#ifndef AG_SDLPAINTER_H
#define AG_SDLPAINTER_H

#include <ag_surface.h>

namespace AGSDLPainter
{
  void drawGradient(SDL_Surface *surface, const AGRect2& prect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  void drawGradientAlpha(SDL_Surface *surface, const AGRect2& prect, const AGColor& ul, const AGColor& ur, const AGColor& dl, const AGColor& dr);
  void drawBorder(SDL_Surface *surface, const AGRect2& rect,int width, const AGColor& c1, const AGColor& c2);

  void drawLine(SDL_Surface *s,const AGVector2 &pp0,const AGVector2 &pp1,const AGColor &c);
}


#endif

