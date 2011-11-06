#include "vertex_array.h"
#include <SDL_opengl.h>
#include <assert.h>
#include <ag_vdebug.h>
#include <ag_profiler.h>
#include <ag_config.h>
#include <ag_main.h>

static bool useVBO()
  {
    AGString s=getConfig()->get("useVBO");
    //cdebug("useVBO:"<<s);
    if(s!="true" && s!="false")
      {
        cdebug("useVBO:"<<s);
        s="true";
        getConfig()->set("useVBO",s);
      }
    return s=="true";
  }

static bool useVertexArrays()
  {
    AGString s=getConfig()->get("useVertexArrays");
    if(s!="true" && s!="false")
      {
        s="true";
        getConfig()->set("useVertexArrays",s);
      }
    return s=="true";
  }


VertexArray::VertexArray(bool pDynamic):mDynamic(pDynamic),bbox(AGVector3(),AGVector3())
{
  bColor=true;
  GLeeInit();
  mBuffers=GLEE_ARB_vertex_buffer_object && useVBO();
  mArrays=GLEE_EXT_vertex_array && useVertexArrays();

  if(videoInited())
    assert(GLEE_EXT_vertex_array || GLEE_VERSION_1_2); // vertex-arrays were introduced in GL 1.1, but glee has no flag for  that

  displayListInited=false;

  mChanged=false;

  mVertexBuffer=0;
  mColorBuffer=0;
  mNormalBuffer=0;
  mTexBuffer=0;
  mIndexBuffer=0;
  mTextures3D=false;
}

VertexArray::~VertexArray()
  {
    //  CTRACE;
    if(mBuffers && mVertexBuffer!=0) //mChanged==false)
      {
        glDeleteBuffersARB( 1, &mVertexBuffer );
        glDeleteBuffersARB( 1, &mColorBuffer );
        glDeleteBuffersARB( 1, &mNormalBuffer );
        glDeleteBuffersARB( 1, &mTexBuffer );
        if(!mDynamic)
          glDeleteBuffersARB( 1, &mIndexBuffer );
      }
  }


void VertexArray::addVertex(AGVector4 pVertex, AGVector4 pColor, AGVector3 pNormal, AGVector2 pTex)
  {
    mVertices.push_back(pVertex);
    mColors.push_back(pColor);
    mNormals.push_back(pNormal);
    mTexCoords.push_back(pTex);
    mChanged=true;
    if(mVertices.size()==1)
      bbox=AGBox3(pVertex.dim3(),AGVector3(0,0,0));
    else
      bbox.include(pVertex.dim3());
  }

void VertexArray::addVertex(AGVector4 pVertex, AGVector4 pColor, AGVector3 pNormal, AGVector3 pTex)
  {
    assert(mTextures3D || mVertices.size()==0);
    mTextures3D=true;
    mVertices.push_back(pVertex);
    mColors.push_back(pColor);
    mNormals.push_back(pNormal);
    mTexCoords3D.push_back(pTex);
    mChanged=true;
    if(mVertices.size()==1)
      bbox=AGBox3(pVertex.dim3(),AGVector3(0,0,0));
    else
      bbox.include(pVertex.dim3());
  }

void VertexArray::addTriangle(size_t p0,size_t p1,size_t p2)
  {
    assert(p0<mVertices.size());
    assert(p1<mVertices.size());
    assert(p2<mVertices.size());
    mIndices.push_back(p0);
    mIndices.push_back(p1);
    mIndices.push_back(p2);
    mChanged=true;
  }

void VertexArray::init()
  {
    if(mBuffers)
      {
        STACKTRACE;
        glGenBuffersARB( 1, &mVertexBuffer );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, mVertices.size()*sizeof(AGVector4), &(mVertices[0]), GL_STATIC_DRAW_ARB );

        glGenBuffersARB( 1, &mColorBuffer );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mColorBuffer );
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, mColors.size()*sizeof(AGVector4), &(mColors[0]), GL_STATIC_DRAW_ARB );

        glGenBuffersARB( 1, &mNormalBuffer );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mNormalBuffer );
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, mNormals.size()*sizeof(AGVector3), &(mNormals[0]), GL_STATIC_DRAW_ARB );

        glGenBuffersARB( 1, &mTexBuffer );
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mTexBuffer );
        if(mTextures3D)
          glBufferDataARB( GL_ARRAY_BUFFER_ARB, mTexCoords3D.size()*sizeof(AGVector3), &(mTexCoords3D[0]), GL_STATIC_DRAW_ARB );
        else
          glBufferDataARB( GL_ARRAY_BUFFER_ARB, mTexCoords.size()*sizeof(AGVector2), &(mTexCoords[0]), GL_STATIC_DRAW_ARB );

        if(!mDynamic)
          {
            glGenBuffersARB( 1, &mIndexBuffer );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
            glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndices.size()*sizeof(Uint16), &(mIndices[0]), GL_STATIC_DRAW_ARB );
          }

        glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


      }
    mChanged=false;
  }

void VertexArray::setColors(bool color)
  {
    bColor=color;
  }

void VertexArray::draw()
  {
    assertGL;
    if(mChanged)
      init();

    assertGL;

    if(mBuffers||mArrays)
      {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        if(bColor)
          glEnableClientState(GL_COLOR_ARRAY);
        else
          glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      }

    if(mBuffers)
      {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mNormalBuffer );
        glNormalPointer(GL_FLOAT, 0, 0);

        /*
      glClientActiveTexture(GL_TEXTURE1);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, mTexBuffer);
      glTexCoordPointer(2, GL_FLOAT, 0, 0);
         */
        glClientActiveTexture(GL_TEXTURE0);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mTexBuffer);
        if(mTextures3D)
          glTexCoordPointer(3, GL_FLOAT, 0, 0);
        else
          glTexCoordPointer(2, GL_FLOAT, 0, 0);

        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBuffer);
        glVertexPointer(4, GL_FLOAT, 0, 0);
        if(bColor)
          {
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, mColorBuffer);
            glColorPointer(4, GL_FLOAT, 0, 0);
          }

        if(mDynamic)
          {
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
                &(mIndices[0]));
          }
        else
          {
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer);
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,0);
          }
      }
    else if(mArrays)
      {
        //      cdebug("too much work");
        glNormalPointer(GL_FLOAT, 0, &(mNormals[0]));
        glClientActiveTexture(GL_TEXTURE0);
        if(mTextures3D)
          glTexCoordPointer(3, GL_FLOAT, 0, &(mTexCoords3D[0]));
        else
          glTexCoordPointer(2, GL_FLOAT, 0, &(mTexCoords[0]));
        glVertexPointer(4, GL_FLOAT, 0, &(mVertices[0]));
        if(bColor)
          glColorPointer(4, GL_FLOAT, 0, &(mColors[0]));
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
            &(mIndices[0]));
      }
    else
      {
        glClientActiveTexture(GL_TEXTURE0);
        glBegin(GL_TRIANGLES);
        for(std::vector<Uint16>::iterator i=mIndices.begin();i!=mIndices.end();i++)
          {
            glNormal3fv(mNormals[*i]);
            if(mTextures3D)
              glTexCoord3fv(mTexCoords3D[*i]);
            else
              glTexCoord2fv(mTexCoords[*i]);
            if(bColor)
              glColor4fv(mColors[*i]);
            glVertex4fv(mVertices[*i]);
          }
        glEnd();

      }

    if(mBuffers||mArrays)
      {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }
    assertGL;

  }

void VertexArray::drawPick()
  {
    STACKTRACE;
    if(false)
      {
        // use vertex arrays
        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(4, GL_FLOAT, 0, &(mVertices[0]));
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
            &(mIndices[0]));

        glDisableClientState(GL_VERTEX_ARRAY);

      }

    else if(true)
      {
        if(displayListInited)
          {
            STACKTRACE;

            glCallList(displayList);
          }
        else
          {
            displayList=glGenLists(1);
            glNewList(displayList,GL_COMPILE);
              {
                // paint in software - because this is faster for picking
                glBegin(GL_TRIANGLES);
                for(std::vector<Uint16>::iterator i=mIndices.begin();i!=mIndices.end();i++)
                  glVertex4fv(mVertices[*i]);
                glEnd();
              }
              glEndList();
              glCallList(displayList);

              displayListInited=true;
          }
      }
    else
      {
        // FIXME: some this doesn't - what the heck!
        if(mChanged)
          init();


        glEnableClientState(GL_VERTEX_ARRAY);


        // disable textures
        glClientActiveTexture(GL_TEXTURE1);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTexture(GL_TEXTURE0);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

          {
            cdebug(mVertices.size());
            cdebug(mIndices.size());
            glBindBufferARB( GL_ARRAY_BUFFER_ARB,0);
            glNormalPointer(GL_FLOAT,0,0);
            glBindBufferARB( GL_ARRAY_BUFFER_ARB,0);
            glTexCoordPointer(2,GL_FLOAT,0,0);
            glVertexPointer(4, GL_FLOAT, 0, &(mVertices[0]));
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
                &(mIndices[0]));
          }

          glDisableClientState(GL_VERTEX_ARRAY);
      }
  }



void VertexArray::drawDepth()
  {
#ifdef OLD
    if(mChanged)
      init();

    glEnableClientState(GL_VERTEX_ARRAY);


    //  glDisable(GL_TEXTURE_2D);

    if(mBuffers)
      {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBuffer);
        glVertexPointer(4, GL_FLOAT, 0, 0);

        if(mDynamic)
          {
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
                &(mIndices[0]));
          }
        else
          {
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer);
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,0);
          }

      }
    else
      {
        //      cdebug("too much work");
        glVertexPointer(4, GL_FLOAT, 0, &(mVertices[0]));

        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
            &(mIndices[0]));
      }

    //  glEnable(GL_TEXTURE_2D);

    glDisableClientState(GL_VERTEX_ARRAY);
#else
    if(mChanged)
      init();

    glEnableClientState(GL_VERTEX_ARRAY);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);


    if(mBuffers)
      {
        glClientActiveTexture(GL_TEXTURE0);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mTexBuffer);
        if(mTextures3D)
          glTexCoordPointer(3, GL_FLOAT, 0, 0);
        else
          glTexCoordPointer(2, GL_FLOAT, 0, 0);

        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBuffer);
        glVertexPointer(4, GL_FLOAT, 0, 0);

        if(mDynamic)
          {
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
                &(mIndices[0]));
          }
        else
          {
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer);
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,0);
          }

      }
    else
      {
        if(mTextures3D)
          glTexCoordPointer(3, GL_FLOAT, 0, &(mTexCoords3D[0]));
        else
          glTexCoordPointer(2, GL_FLOAT, 0, &(mTexCoords[0]));
        glVertexPointer(4, GL_FLOAT, 0, &(mVertices[0]));

        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT,
            &(mIndices[0]));
      }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
  }



AGVector4 VertexArray::lineHit(const AGLine3 &pLine) const
{
  AGVector4 mfalse(0,0,0,0);
  //  cdebug("box:"<<bbox.toString());
  if(!bbox.collides(pLine))
    return mfalse;

  for(size_t i=0;i<mIndices.size();i+=3)
    {
      AGVector4 v0=mVertices[mIndices[i]];
      AGVector4 v1=mVertices[mIndices[i+1]];
      AGVector4 v2=mVertices[mIndices[i+2]];

      AGTriangle3 t(v0.dim3(),v1.dim3(),v2.dim3());
      AGVector4 r;
      r=t.collide(pLine);
      if(r[3]==1)
        {
          return r;
        }
    }
  return mfalse;
}

void VertexArray::update()
  {
    if(mBuffers && mVertexBuffer!=0) //mChanged==false)
      {
        glDeleteBuffersARB( 1, &mVertexBuffer );
        glDeleteBuffersARB( 1, &mColorBuffer );
        glDeleteBuffersARB( 1, &mNormalBuffer );
        glDeleteBuffersARB( 1, &mTexBuffer );
        glDeleteBuffersARB( 1, &mIndexBuffer );
        mVertexBuffer=0;
      }
    mChanged=true;
  }
void VertexArray::clear()
  {
    mVertices.clear();
    mColors.clear();
    mNormals.clear();
    mTexCoords.clear();
    mTexCoords3D.clear();
    mIndices.clear();
    update();
  }

void VertexArray::setBuffers(bool pBuffers)
  {
    mBuffers=false;
  }


size_t VertexArray::getIndex(size_t i)
  {
    return mIndices[i];
  }
AGVector4 VertexArray::getVertex(size_t i)
  {
    return mVertices[i];
  }
AGVector4 VertexArray::getColor(size_t i)
  {
    return mColors[i];
  }
AGVector3 VertexArray::getNormal(size_t i)
  {
    return mNormals[i];
  }
AGVector2 VertexArray::getTexCoord(size_t i)
  {
    return mTexCoords[i];
  }

void VertexArray::setTexCoord(size_t i,const AGVector2 &t)
  {
    mTexCoords[i]=t;
    update();
  }

bool VertexArray::useVertexArrays() const
{
  return mArrays;
}


void VertexArray::onScreenUp()
  {
    init();
  }

void VertexArray::onScreenDown()
  {
    if(mBuffers && mVertexBuffer!=0) //mChanged==false)
      {
        glDeleteBuffersARB( 1, &mVertexBuffer );
        glDeleteBuffersARB( 1, &mColorBuffer );
        glDeleteBuffersARB( 1, &mNormalBuffer );
        glDeleteBuffersARB( 1, &mTexBuffer );
        if(!mDynamic)
          glDeleteBuffersARB( 1, &mIndexBuffer );
      }
    mVertexBuffer=mColorBuffer=mNormalBuffer=mTexBuffer=mIndexBuffer=0;
  }




////////////////////////////////////////////////////////////////////
// VertexArrayShader
////////////////////////////////////////////////////////////////////



VertexArrayShader::VertexArrayShader(AntShaderProgram *_p):p(_p)
{
  aInited=false;
}
VertexArrayShader::~VertexArrayShader()
  {
    for(std::map<std::string,std::vector<float>*>::iterator i=as.begin();i!=as.end();++i)
      {
        if(i->second)
          checkedDelete(i->second);
      }

    for(std::map<std::string,std::vector<Uint16>*>::iterator i=elementAs.begin();i!=elementAs.end();++i)
      {
        if(i->second)
          checkedDelete(i->second);
      }

  }

void VertexArrayShader::addAttribute(const std::string &pName,const std::vector<float> &a)
  {
    as[pName]=new std::vector<float>(a);
    aInited=false;
  }

void VertexArrayShader::addAttribute(const std::string &pName,const std::vector<Uint16> &a)
  {
    elementAs[pName]=new std::vector<Uint16>(a);
    aInited=false;
  }


void VertexArrayShader::onScreenUp()
  {
    CTRACE;
    VertexArray::onScreenUp();
    
  }
void VertexArrayShader::onScreenDown()
  {
    CTRACE;
    VertexArray::onScreenDown();
    aInited=false;
    // FIXME: cleanup gl-objects
  }


void VertexArrayShader::draw()
  {
    assertGL;
    p->enable();
    assertGL;
    if(!aInited)
      aInit();
    assertGL;
    attach();
    assertGL;
    VertexArray::draw();
    p->disable();
  }

void VertexArrayShader::attach()
  {
    //  if(useVertexArrays())
      {
        for(std::map<std::string,unsigned int>::iterator i=aids.begin();i!=aids.end();i++)
          {
            GLint loc=p->getAttr(i->first);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableVertexAttribArrayARB(loc); // add array
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, i->second);
            glVertexAttribPointerARB(loc,1,GL_FLOAT,0,0,0);
            //      glTexCoordPointer(2, GL_FLOAT, 0, 0);
          }

        for(std::map<std::string,unsigned int>::iterator i=elementIds.begin();i!=elementIds.end();i++)
          {
            assertGL;
            GLint loc=p->getAttr(i->first);
            assertGL;
            glEnableClientState(GL_VERTEX_ARRAY);
            assertGL;
            glEnableVertexAttribArrayARB(loc); // add array
            assertGL;
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, i->second);
            assertGL;
            glVertexAttribPointerARB(loc,1,GL_UNSIGNED_SHORT,0,0,0);
            assertGL;
            //      glTexCoordPointer(2, GL_FLOAT, 0, 0);
          }

      }
  }

void VertexArrayShader::aInit()
  {
    for(std::map<std::string,std::vector<float>*>::iterator i=as.begin();i!=as.end();i++)
      {
        unsigned int id;

        glGenBuffersARB( 1, &id);
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, id);
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, i->second->size()*sizeof(float), &((*i->second)[0]), GL_STATIC_DRAW_ARB );

        aids[i->first]=id;
      }

    for(std::map<std::string,std::vector<Uint16>*>::iterator i=elementAs.begin();i!=elementAs.end();i++)
      {
        unsigned int id;

        glGenBuffersARB( 1, &id);
        glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, id);
        glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, i->second->size()*sizeof(Uint16), &((*i->second)[0]), GL_STATIC_DRAW_ARB );

        elementIds[i->first]=id;
      }


    aInited=true;
  }


VertexArray *makeInstances(const VertexArray &va,const std::vector<AGMatrix4> &ts)
  {
    VertexArray *na=new VertexArray;
    size_t start=0;
    size_t ci;

    for(std::vector<AGMatrix4>::const_iterator i=ts.begin();i!=ts.end();i++)
      {
        assert(va.mVertices.size()==va.mNormals.size());
        // transform normals and vertices
        for(size_t j=0;j<va.mVertices.size();j++)
          {
            AGVector4 v=va.mVertices[j];
            AGVector3 n=va.mNormals[j];

            v=*i*v;
            n=(*i*AGVector4(n,0)).dim3();

            na->mVertices.push_back(v);
            na->mNormals.push_back(n);
          }

        std::copy(va.mColors.begin(),va.mColors.end(),std::back_inserter(na->mColors));
        std::copy(va.mTexCoords.begin(),va.mTexCoords.end(),std::back_inserter(na->mTexCoords));
        std::copy(va.mTexCoords3D.begin(),va.mTexCoords3D.end(),std::back_inserter(na->mTexCoords3D));
        for(std::vector<Uint16>::const_iterator j=va.mIndices.begin();j!=va.mIndices.end();j++)
          {
            ci=*j+start;
            assert(ci>=start);
            assert(ci<na->mVertices.size());
            assert(ci<na->mColors.size());
            assert(ci<na->mNormals.size());

            na->mIndices.push_back(*j+start);
          }
        start+=va.mVertices.size();
      }

    na->bColor=va.bColor;
    na->mChanged=true;

    // calc bbox
    na->bbox=AGBox3(AGVector3(),AGVector3());



    return na;
  }
