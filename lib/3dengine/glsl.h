#ifndef ANT_GLSL_H
#define ANT_GLSL_H

#include "rk_tools.h"

#include "ag_geometry.h"
#include "ag_glscreen.h"
#include "ant_renderer.h"

#include <map>

class Scene;

class AGEXPORT AntVertexProgram {
public:
  AntVertexProgram(const std::string &pFile);
  ~AntVertexProgram();

private:
  GLhandleARB vertexShader;

  friend class AntShaderProgram;
};

class AGEXPORT AntFragProgram {
public:
  AntFragProgram(const std::string &pFile);
  AntFragProgram();
  ~AntFragProgram();

  bool valid() const;

private:
  GLhandleARB fragShader;
  bool mValid;

  friend class AntShaderProgram;
};

class AGEXPORT AntShaderParameter {
public:
};

class AGEXPORT AntShaderProgram : public AGGLObject {
  std::string mVertexFile, mFragFile;
  AntVertexProgram *vertex;
  AntFragProgram *frag;
  std::map<std::string, GLint> locations;
  std::map<std::string, GLint> attrs;
  float *matrixBuf;
  bool on;
  std::string name;

protected:
  GLhandleARB p;

public:
  AntShaderProgram(const std::string &pVertexFile,
                   const std::string &pFragFile);
  virtual ~AntShaderProgram();
  virtual void enable(Renderer *renderer);
  virtual void disable();

  void update(Renderer *renderer, float time);
  virtual void doUpdate(Renderer * renderer, float time);

  GLint getLoc(const std::string &pName);
  GLint getAttr(const std::string &pName);

  void sendUniform(const std::string &pName, int i);
  void sendUniform(const std::string &pName, float f);
  void sendUniform(const std::string &pName, const AGVector3 &m);
  void sendUniform(const std::string &pName, const AGVector4 &m);
  void sendUniform(const std::string &pName, const AGMatrix4 &m);
  void sendUniform(const std::string &pName, const std::vector<AGMatrix4> &m);

  void sendAttribute(const std::string &pName, const std::vector<float> &vf);

  void onScreenUp();
  void onScreenDown();

private:
  void check();
  void init();
  void takeDown();
};

class AGEXPORT AntShadowShader : public AntShaderProgram {
public:
  AntShadowShader(const std::string &pVertexFile, const std::string &pFragFile);
  void doUpdate(Renderer *renderer, float time);
  virtual void enable(Renderer *renderer);
  virtual void disable(Renderer *renderer);
};

class AGEXPORT AntWaterShader : public AntShaderProgram {
  float t;

public:
  AntWaterShader();
  void doUpdate(Renderer * renderer, float time);
};

AGEXPORT bool glslOk();

#endif
