#include "mesh_2d_data.h"

Mesh2DData::Mesh2DData(AGTexture *pTexture):
  mTexture(pTexture)
{
}
  
AGTexture *Mesh2DData::getTexture()
{
  return mTexture;
}

AGBox3 Mesh2DData::bbox() const
{
  // FIXME
  return AGBox3();
}
