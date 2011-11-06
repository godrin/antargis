#include "mesh_data.h"
#include <ag_texturecache.h>
#include "mesh_optimizer.h"
#include <ag_rendercontext.h>

#include <ag_serial.h>
#include <ag_main.h>
#include <ag_vdebug.h>

//////////////////////////////////////////////////////////////////////
// MeshData
//////////////////////////////////////////////////////////////////////

int getMeshDownScale(); // implemented in anim_mesh_data.cc


MeshData::MeshData(const VertexArray &va,const std::string &pTexture,bool pShadow):mBBox(AGVector3(0,0,0),AGVector3(-1,0,0))
{
  CTRACE;
  mArray=va;
  mWithTexture=false;
  if(pTexture!="" && videoInited())
    {
      mTexture=getTextureCache()->get(pTexture,getMeshDownScale());
      mWithTexture=true;
    }
  mShadow=pShadow;
  //  mAlpha=false;
  mTransparent=false;
  overdraw=false;
  drawColors=true;
  mPickable=true;
  mLighting=true;
  mCulling=true;
}


MeshData::MeshData(const std::string &filename,float zoom,const std::string &pTexture,bool pShadow):mBBox(AGVector3(0,0,0),AGVector3(-1,0,0))
{
  CTRACE;
  Uint16 faces,meshes,vertices;
  overdraw=false;

  mTransparent=false;
  mLighting=true;
  mCulling=true;

  //  mAlpha=false;

  mWithTexture=false;
  if(pTexture!="" && videoInited())
    {
      mTexture=getTextureCache()->get(pTexture,getMeshDownScale());
      mWithTexture=true;
    }

  MeshVertex mVertices[4];
  MeshOptimizer opt;
  bool withTex=filename.find(".ant2")!=filename.npos;

  BinaryFileIn f(filename);
  /*

  FILE *f=fopen(filename.c_str(),"rb");
  assert(f);
  fread(&meshes,sizeof(Uint16),1,f);*/

  f>>meshes;

  for(;meshes>0;meshes--)
    {
      f>>faces;
      //      fread(&faces,sizeof(Uint16),1,f);

      for(Uint16 i=0;i<faces;i++)
        {
          //  fread(&vertices,sizeof(Uint16),1,f);
          f>>vertices;
          assert(vertices<=4);
          for(Uint16 j=0;j<vertices;j++)
            {
              AGVector3 v,n,c;
              f>>v;
              mVertices[j].v=AGVector4(v,1);
              f>>n;
              mVertices[j].n=n;
              f>>c; // load here first, as mVertices[.].c is a AGVector4
              mVertices[j].c=AGVector4(c,1);
              if(withTex)
                f>>mVertices[j].t;

              mVertices[j].t[1]=1-mVertices[j].t[1];

              mVertices[j].v*=zoom;
              mVertices[j].v[3]=1;

              mBBox.include(mVertices[j].v.dim3());
            }
          if(vertices==3)
            {
              opt.add(mVertices[0]);
              opt.add(mVertices[1]);
              opt.add(mVertices[2]);
            }
          else
            {
              opt.add(mVertices[0]);
              opt.add(mVertices[1]);
              opt.add(mVertices[2]);
              opt.add(mVertices[0]);
              opt.add(mVertices[2]);
              opt.add(mVertices[3]);
            }
        }
    }
  //  fclose(f);
  mShadow=pShadow;
  mArray=opt.getArray();




  drawColors=true;
  mPickable=true;
}

MeshData::~MeshData() throw()
  {
    //  TRACE;
  }

bool MeshData::transparent()
  {
    return mTransparent;
  }

void MeshData::setTransparent(bool p)
  {
    mTransparent=p;
  }

void MeshData::setLighting(bool l)
  {
    mLighting=l;
  }

AGBox3 MeshData::bbox() const
{
  return mBBox;
}


void MeshData::save(const std::string &pFilename)
  {
    FILE *f=fopen(pFilename.c_str(),"wb");

    size_t meshes=1;
    size_t vertices=3;
    size_t faces=mArray.getTriangles();

    // meshes
    fwrite(&meshes,sizeof(Uint16),1,f);

    // faces
    fwrite(&faces,sizeof(Uint16),1,f);
    for(size_t i=0;i<faces;i++)
      {
        fwrite(&vertices,sizeof(Uint16),1,f);
        for(size_t k=0;k<vertices;k++)
          {
            size_t j=mArray.getIndex(i*vertices+k);
            fwrite(mArray.getVertex(j),sizeof(float),3,f);
            fwrite(mArray.getNormal(j),sizeof(float),3,f);
            fwrite(mArray.getColor(j),sizeof(float),3,f);
            fwrite(mArray.getTexCoord(j),sizeof(float),2,f);
          }
      }

    fclose(f);
  }


void MeshData::setOverdraw(bool o)
  {
    overdraw=o;
  }

void MeshData::drawPick()
  {
    if(mPickable)
      mArray.drawPick();
  }
#ifdef OLD
void MeshData::draw()
  {
    if(!mShadow)
      glDepthMask(false);

    if(mTransparent)
      glDisable(GL_CULL_FACE);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.9);


    if(mLighting)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    glBindTexture(GL_TEXTURE_2D,0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);


    if(mWithTexture)
      glBindTexture(GL_TEXTURE_2D,mTexture.getTextureID());

    if(mTransparent)
      {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//_MIPMAP_LINEAR);//NEAREST);//GL_LINEAR);

        //      glDisable(GL_LIGHTING);
        glAlphaFunc(GL_GREATER,0.4f);
        glEnable(GL_ALPHA_TEST);
      }
    if(overdraw)
      {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_DEPTH_TEST);
      }
    mArray.setColors(drawColors);
    mArray.draw();
    if(overdraw)
      {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
      }
    if(mTransparent)
      {
        glAlphaFunc(GL_GREATER,0.9f);
        glEnable(GL_LIGHTING);
      }
    //    glEnable(GL_ALPHA_TEST);


    glBindTexture(GL_TEXTURE_2D,0);
    if(!mShadow)
      glDepthMask(true);

    glDisable(GL_ALPHA_TEST);
    if(mTransparent)
      glEnable(GL_CULL_FACE);
  }
#else

void MeshData::setCulling(bool c)
  {
    mCulling=c;
  }


void MeshData::draw(const AGVector4 &pColor)
  {
    AGRenderContext c;

    if(!mShadow)
      c.setDepthWrite(false);

    //  if(mTransparent)
    c.setCulling(mCulling);

    c.setAlpha(0.9,GL_GREATER);

    c.setLighting(mLighting);

    if(pColor!=AGVector4(1,1,1,1))
      c.setColor(pColor);

    if(mWithTexture)
      c.setTexture(mTexture.glTexture());

    if(mTransparent)
      {
        mTexture.setFilter(GL_LINEAR,GL_LINEAR);
        c.setAlpha(0.4f,GL_GREATER);
      }
    if(overdraw)
      {
        c.setDepthWrite(false);
        c.setDepthTest(false);
        c.setAlpha(0,GL_NONE);
      }
    c.begin();
    mArray.setColors(drawColors);
    mArray.draw();
  }

#endif


void MeshData::setColors(bool c)
  {
    drawColors=c;
  }

size_t MeshData::getTriangles() const
{
  return mArray.getTriangles();
}

void MeshData::drawShadow()
  {
    if(mTransparent)
      glDisable(GL_CULL_FACE);

    drawDepth();

    if(mTransparent)
      glEnable(GL_CULL_FACE);
  }
void MeshData::drawDepth()
  {
    assertGL;
    AGRenderContext c;
    if(mTransparent)
      {
        c.setCulling(false);
        c.setAlpha(0.9f,GL_GREATER);
        c.setTexture(mTexture.glTexture());
        /*
      glDisable(GL_CULL_FACE);
      //      throw int();

      glBindTexture(GL_TEXTURE_2D,mTexture.getTextureID());
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER,0.9f);
         */
        //      cdebug("muh");
      }
    c.begin();
    assertGL;
    if(mShadow)
      {
        mArray.setColors(false);
        mArray.draw();
        mArray.setColors(true);
      }
    /*  if(mTransparent)
    {
      glEnable(GL_CULL_FACE);
      glBindTexture(GL_TEXTURE_2D,0);
      }*/
  }

void MeshData::texCoordFromPos(float scale)
  {
    for(size_t i=0;i<mArray.getTriangles()*3;i++)
      {
        size_t j=mArray.getIndex(i);

        AGVector4 v=mArray.getVertex(j);
        AGVector2 t(v[0]+v[1],v[2]);
        mArray.setTexCoord(j,t*scale);
      }
  }

void MeshData::setPickable(bool p)
  {
    mPickable=p;
  }

void MeshData::makeInstances(const std::vector<AGMatrix4> &ts)
  {
    VertexArray *va=::makeInstances(mArray,ts);
    mArray=*va;
    checkedDelete(va);
  }
