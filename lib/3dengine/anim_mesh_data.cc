#include "anim_mesh_data.h"
#include "ag_texturecache.h"
#include "ag_config.h"
#include "ag_xml.h"

#include <ag_serial.h>

void setRotation(AGMatrix4 &m,const AGVector3 &angles); // from anim_mesh.cc
void setTranslation(AGMatrix4 &m,const AGVector3 &t);
void inverseRotate(AGVector3 &v, const AGMatrix4 &m );
void inverseTranslate(AGVector3 &v, const AGMatrix4 &m );


bool useAnimation()
  {
    bool anim=getConfig()->get("animationType")!="false";
    getConfig()->set("animationType",anim?"true":"false");
    return anim;
  }

bool useShaderAnimation()
  {
    return glslOk() && GLEE_ARB_vertex_buffer_object; // doesn't work with vbos
  }

AGVector3 Bone::interpolate(const std::vector<KeyFrame> &frames,float t)
  {
    size_t i;
    for(i=0;i<frames.size();i++)
      {
        if(frames.size()>i+1)
          {
            if(frames[i+1].time>t)
              break;
          }
        else
          break;
      }
    size_t j=i+1;
    if(j>=frames.size())
      return frames[i].v;

    float t0=frames[i].time;
    float t1=frames[j].time;

    float i0=(t-t0)/(t1-t0);
    float i1=1-i0;

    return frames[i].v*i1 + frames[j].v*i0;
  }
AGVector3 Bone::interpolateRot(float t)
  {
    return interpolate(rFrames,t);
  }
AGVector3 Bone::interpolateTrans(float t)
  {
    return interpolate(tFrames,t);
  }



AnimMeshData::AnimMeshData(const AGString &xmlFilename):
  animShader("data/shaders/anim.vert","data/shaders/anim.frag"),
  //  animShaderDepth("data/shaders/anim_depth.vert",""),
  animShaderDepth("data/shaders/anim_depth.vert","data/shaders/anim_depth.frag"),
  mArray(&animShader),
  mArrayDepth(&animShaderDepth)
  {
    Document doc(xmlFilename);

    Node &root=doc.root();

    events=false;

    animate=true;
    if(root.get("debug")=="true")
      animate=false;
    if(!useAnimation())
      animate=false;

    cdebug("loading from:"<<root.get("model"));

    loadAnt3(loadFile(root.get("model")),root.get("scale").toFloat(),root.get("texture"));

    // load animations

    cdebug("animTime:"<<animTime);

    Node::NodeVector anims=root.getChildren("animation");
    for(Node::NodeVector::iterator i=anims.begin();i!=anims.end();i++)
      {
        float begin=(*i)->get("begin").toFloat()-1;
        float end=(*i)->get("end").toFloat()-1;
        float fps=(*i)->get("fps").toFloat();
        assert(fps>0);
        assert(begin>=0);
        assert(begin<animTime);
        assert(begin<=end);
        assert(end<animTime);
        mAnimations[(*i)->get("name")]=Animation(begin,end,fps);

        mAnimations[(*i)->get("name")].loop=((*i)->get("loop")!="false");

        // parse events
        Node::NodeVector enodes=(*i)->getChildren("event");
        for(Node::NodeVector::iterator j=enodes.begin();j!=enodes.end();j++)
          {
            frameEvents.insert(std::make_pair((*j)->get("frame").toInt(),(*j)->get("name")));
          }

      }

    if(anims.size()==0)
      {
        std::cerr<<"There are no animations in "<<xmlFilename<<std::endl;
        throw std::string("no animations found in xmlfile");
      }
    mName=AGString(xmlFilename);
  }

AnimMeshData::~AnimMeshData() throw()
  {
    CTRACE;
    cdebug(mName);

    // delete bones
    for(std::vector<Bone*>::iterator i=bones.begin();i!=bones.end();++i)
      checkedDelete(*i); // checked - no agrubyobject


  }

AGBox3 AnimMeshData::bbox() const
{
  return mBBox;
}

void AnimMeshData::setEvents(bool e)
  {
    events=e;
  }


int getMeshDownScale()
  {
    int s=1;
    AGString n="meshDownScaleExp";

    if(getConfig()->get(n)!="")
      s=getConfig()->get(n).toInt();

    getConfig()->set(n,AGString(s));

    return s;

  }

void AnimMeshData::loadAnt3(const AGString &instr,float scale,const AGString &pTextureFilename)
  {
    mTexture=getTextureCache()->get(pTextureFilename,getMeshDownScale());
    BinaryStringIn l(instr);

    Uint32 vs,ts;

    l>>vs;
    for(Uint32 i=0;i<vs;i++)
      {
        float x,y,z,tx,ty,nx,ny,nz;
        Sint32 boneID;
        l>>x>>y>>z>>tx>>ty>>boneID>>nx>>ny>>nz;

        AGVector3 v(x,y,z);
        v*=scale;
        mBBox.include(v);
        pos.push_back(v);
        uv.push_back(AGVector2(tx,1-ty));
        normal.push_back(AGVector3(nx,ny,nz));
        bone.push_back(boneID);
      }

    l>>ts;
    Uint32 index;
    for(Uint32 i=0;i<ts*3;i++)
      {
        l>>index;
        indices.push_back(index);
      }

    Uint32 nbones,frames;
    l>>frames>>nbones;

    for(size_t k=0;k<bone.size();k++)
      {
        if(bone[k]==-1)
          {
            bonef.push_back(nbones);
            boneui.push_back(nbones);
          }
        else
          {
            bonef.push_back(bone[k]);
            boneui.push_back(bone[k]);
          }
      }


    animTime=frames;

    for(Uint32 i=0;i<nbones;i++)
      {
        float x,y,z,rx,ry,rz;
        Sint32 parent;
        l>>x>>y>>z>>rx>>ry>>rz>>parent;
        Bone *bone=new Bone;
        bone->id=i;
        bone->pos=AGVector3(x,y,z)*scale;
        bone->rot=AGVector3(rx,ry,rz);
        if(parent<0)
          bone->parent=0;
        else
          {
            assert(parent<(int)i);
            bone->parent=bones[parent];
          }

        // read rot-frames
        Sint32 frames;
        l>>frames;
        KeyFrame frame;
        for(Sint32 r=0;r<frames;r++)
          {
            float x,y,z,t;
            l>>t>>x>>y>>z;
            frame.time=t;
            frame.v=AGVector3(x,y,z);
            bone->rFrames.push_back(frame);
          }

        l>>frames;
        for(Sint32 r=0;r<frames;r++)
          {
            float x,y,z,t;
            l>>t>>x>>y>>z;
            frame.time=t;
            frame.v=AGVector3(x,y,z)*scale;
            bone->tFrames.push_back(frame);
          }



        bones.push_back(bone);
      }

    if(animate)
      setupJoints();

    setupArray();
  }

void AnimMeshData::setTransform(const AGMatrix4 &m)
  {
    mBase=m;
  }

const AGMatrix4 &AnimMeshData::getTransform() const
{
  return mBase;
}


void AnimMeshData::setupJoints()
  {
    for (std::vector<Bone*>::iterator i = bones.begin(); i!=bones.end(); i++ )
      {
        Bone& joint = **i;

        setRotation(joint.mRelative, joint.rot);
        setTranslation(joint.mRelative, joint.pos);

        if(joint.parent)
          joint.mAbsolute=joint.parent->mAbsolute*joint.mRelative;
        else
          joint.mAbsolute=joint.mRelative;

      }

    for (size_t i=0;i<pos.size();i++)
      {
        if(bone[i]!=-1)
          {
            AGMatrix4 m(bones[bone[i]]->mAbsolute);
            AGVector3 test=pos[i];
            inverseTranslate(pos[i],m);
            inverseRotate(pos[i],m);

            // normals
            inverseRotate(normal[i],m);
          }
      }
  }


void AnimMeshData::setupArray()
  {
    for(size_t i=0;i<pos.size();i++)
      mArray.addVertex(AGVector4(pos[i],1),AGVector4(1,1,1,1),normal[i],uv[i]);
    for(size_t i=0;i<indices.size();i+=3)
      mArray.addTriangle(indices[i],indices[i+1],indices[i+2]);

    // set boneIDs
    mArray.addAttribute("bones",bonef);
    //  mArray.addAttribute("ibones",boneui);




    for(size_t i=0;i<pos.size();i++)
      mArrayDepth.addVertex(AGVector4(pos[i],1),AGVector4(1,1,1,1),normal[i],uv[i]);
    for(size_t i=0;i<indices.size();i+=3)
      mArrayDepth.addTriangle(indices[i],indices[i+1],indices[i+2]);

    // set boneIDs
    mArrayDepth.addAttribute("bones",bonef);
    //  mArrayDepth.addAttribute("ibones",boneui);


  }

std::vector<std::string> AnimMeshData::getAnimations() const
{
  std::vector<std::string> l;

  for(std::map<AGString,Animation>::const_iterator i=mAnimations.begin();i!=mAnimations.end();i++)
    l.push_back(i->first);

  return l;
}
