#include "ag_sdlpainter.h"
#include <cmath>
#include "ag_sgeexport.h"

namespace AGSDLPainter
{
  void drawLine(SDL_Surface *s,const AGVector2 &pp0,const AGVector2 &pp1,const AGColor &c)
    {
      AGVector2 p0,p1;

      p0=pp0;
      p1=pp1;

      float dx=p1[0]-p0[0];
      float dy=p1[1]-p0[1];

      if(fabs(dx)>fabs(dy))
        {
          if(dx<0)
            {
              p0=pp1;
              p1=pp0;
            }

          dx=p1[0]-p0[0];
          dy=p1[1]-p0[1];

          float y=p0[1];
          float e=0;
          float de=((float)dy)/dx;
          for(float x=p0[0];x<=p1[0];x++)
            {
              sge_PutPixel(s,int(x),int(y),c.mapRGB(s->format));
              e+=de;
              if(e>0.5)
                {
                  e-=1.0;
                  y++;
                }
              else if(e<-0.5)
                {
                  e+=1.0;
                  y--;
                }
            }
        }
      else
        {
          if(dy<0)
            {
              p0=pp1;
              p1=pp0;
            }

          dx=p1[0]-p0[0];
          dy=p1[1]-p0[1];

          float x=p0[0];
          float e=0;
          float de=((float)dx)/dy;
          for(float y=p0[1];y<=p1[1];y++)
            {
              sge_PutPixel(s,int(x),int(y),c.mapRGB(s->format));
              e+=de;
              if(e>0.5)
                {
                  e-=1.0;
                  x++;
                }
              else if(e<-0.5)
                {
                  e+=1.0;
                  x--;
                }
            }
        }
    }


}
