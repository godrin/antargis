#include "mesh_optimizer.h"
#include <rk_debug.h>
#include <ag_serial.h>

#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Mesh-optimizing
//////////////////////////////////////////////////////////////////////

// sorting
bool MeshVertex::operator<(const MeshVertex &p) const
{
  int i;
  for(i=0;i<3;i++)
    if(v[i]<p.v[i])
      return true;

  for(i=0;i<4;i++)
    if(c[i]<p.c[i])
      return true;

  for(i=0;i<3;i++)
    if(n[i]<p.n[i])
      return true;

  for(i=0;i<2;i++)
    if(t[i]<p.t[i])
      return true;

  return false;
}


MeshOptimizer::MeshOptimizer()
  {
    saved=0;
  }
void MeshOptimizer::add(const MeshVertex &v)
  {
    size_t index=mVertices.size();
    std::map<MeshVertex,int>::iterator i=mMap.find(v);
    if(i!=mMap.end())
      {
        saved++;
        index=i->second;
      }
    else
      {
        mVertices.push_back(v);
        mMap[v]=index;
      }
    assert(index<mVertices.size());
    mIndices.push_back(index);
    return;
  }


struct MeshTriangle
{
  size_t i0,i1,i2;
  MeshTriangle(size_t p0,size_t p1,size_t p2)
    {
      i0=p0;
      i1=p1;
      i2=p2;
    }

  bool operator<(const MeshTriangle &t) const;
};

struct TriCompare
{
  MeshOptimizer *o;
  AGVector3 c;
  TriCompare(MeshOptimizer *p,AGVector3 cam):o(p),c(cam)
  {
  }
  bool operator()(const MeshTriangle &t1,const MeshTriangle &t2)
  {
    AGVector3 m0=(o->getV(t1.i0).dim3()+o->getV(t1.i1).dim3()+o->getV(t1.i2).dim3())/3.0;
    AGVector3 m1=(o->getV(t2.i0).dim3()+o->getV(t2.i1).dim3()+o->getV(t2.i2).dim3())/3.0;

    return (c-m0).length2()>(c-m1).length2();
  }
};



VertexArray MeshOptimizer::getArray()
  {
    CTRACE;
    VertexArray a;
    for(size_t i=0;i<mVertices.size();i++)
      {
        MeshVertex v=mVertices[i];
        a.addVertex(v.v,v.c,v.n,v.t);
      }

    std::vector<MeshTriangle> tris;
    for(size_t i=0;i<mIndices.size();i+=3)
      {
        tris.push_back(MeshTriangle(mIndices[i],mIndices[i+1],mIndices[i+2]));
      }
    sort(tris.begin(),tris.end(),TriCompare(this,AGVector3(0,-10,10)));

    for(std::vector<MeshTriangle>::iterator i=tris.begin();i!=tris.end();i++)
      a.addTriangle(i->i0,i->i1,i->i2);


    if(mIndices.size()==0)
      cdebug("NO TRIANGLES FOUND!");
    cdebug("SAVED:"<<1.0f-float(saved)/mIndices.size());
    return a;
  }

AGVector4 MeshOptimizer::getV(size_t i)
  {
    return mVertices[i].v;
  }


MeshOptimizer loadFromText(const std::string &pText, bool withTex, float zoom)
  {
    MeshOptimizer opt;
    MeshVertex mVertices[4];

    BinaryStringIn in(pText);

    Uint16 meshes,faces,vertices;
    AGVector3 v;

    in>>meshes;
    cdebug("meshes:"<<meshes);
    for(;meshes>0;meshes--)
      {
        in>>faces;
        cdebug("faces:"<<faces);
        for(Uint16 i=0;i<faces;i++)
          {
            in>>vertices;
            cdebug("vs:"<<vertices);
            assert(vertices<=4);
            for(Uint16 j=0;j<vertices;j++)
              {
                in>>v;
                mVertices[j].v=AGVector4(v,1);
                // cdebug("v:"<<mVertices[j].v.toString());
                in>>mVertices[j].n;
                in>>v;
                mVertices[j].c=AGVector4(v,1);
                if(withTex)
                  in>>mVertices[j].t;
                mVertices[j].v*=zoom;
                mVertices[j].v[3]=1;
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


    return opt;
  }
