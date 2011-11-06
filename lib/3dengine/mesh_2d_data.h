#ifndef MESH_2D_DATA
#define MESH_2D_DATA

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <ag_texture.h>

class AGEXPORT Mesh2DData
{
 public:
  Mesh2DData(AGTexture *pTexture);
  
  AGTexture *getTexture();

  AGBox3 bbox() const;

 private:
  AGTexture *mTexture;
};

#endif
