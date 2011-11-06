#ifndef ANT_PARTICLE_H
#define ANT_PARTICLE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "scenenode.h"
#include "ag_texture.h"

/**
   some very simple particle engine for displaying
   smoke and ugly fire.
*/
class AGEXPORT AntParticle:public SceneNode
{
  /// this represents one single particle
  struct Piece
  {
    AGVector3 pos;
    AGVector3 speed;
    AGVector3 color;
    float lived;
    float light;
    float size;
    
  };

  std::list<Piece*> mPieces;
  float maxtime,freq;
  float mtime;
  float stime;
  bool fire;
  bool enabled;

  AGTexture mTexture;


 public:
  /// make some smoke (or fire) in Scene pScene with output frequency f
  AntParticle(Scene *pScene,float f);
  
  virtual ~AntParticle() throw();

  /// if f is true this object will display a fire - otherwise it's smoke
  /// default is smoke
  void setFire(bool f);
  /// maximal time for a particle to live
  void setMaxTime(float m);

  /// advance the smoke-particles; generate new ones; discard old ones
  void advance(float time);

  /// let Scene take care of drawing order for transparency
  virtual bool transparent()
  {
    return true;
  }

  void draw();

  /// disable the generation of new particles (smoke will fade slowly)
  void setEnabled(bool f);

};

#endif
