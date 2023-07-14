#include "glsl.h"
#include "ag_main.h"
#include "ag_vdebug.h"
#include "ant_renderer.h"
#include "scene.h"

bool glslOk();
void printInfoLog(GLhandleARB obj);

AntVertexProgram::AntVertexProgram(const std::string &pFile) {
  assertGL;
  if (glslOk()) {
    vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    std::string s = loadFile(pFile);

    const char *a = s.c_str();
    const GLcharARB **x = &a;
    glShaderSourceARB(vertexShader, 1, x, NULL);
    glCompileShaderARB(vertexShader);
    printInfoLog(vertexShader);
  }
  assertGL;
}

AntVertexProgram::~AntVertexProgram() {
  assertGL;
  if (glslOk() && !hasQuit())
    glDeleteObjectARB(vertexShader);
  assertGL;
}

AntFragProgram::AntFragProgram(const std::string &pFile) {
  assertGL;
  mValid = false;
  if (glslOk() && pFile.length() > 0) {
    fragShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    std::string s = loadFile(pFile);
    const char *a = s.c_str();
    const GLcharARB **x = &a;
    glShaderSourceARB(fragShader, 1, x, NULL);
    glCompileShaderARB(fragShader);
    printInfoLog(fragShader);
    mValid = true;
  }
  assertGL;
}

AntFragProgram::AntFragProgram() { mValid = false; }

AntFragProgram::~AntFragProgram() {
  assertGL;
  if (glslOk() && !hasQuit())
    glDeleteObjectARB(fragShader);
  assertGL;
}

bool AntFragProgram::valid() const { return mValid; }

AntShaderProgram::AntShaderProgram(const std::string &pVertexFile,
                                   const std::string &pFragFile)
    : //  vertex(getVertexProgram(pVertexFile)),frag(getFragProgram(pFragFile))
      mVertexFile(pVertexFile), mFragFile(pFragFile), vertex(0), frag(0) {
  on = false;
  matrixBuf = new float[16 * 100];
  name = pVertexFile + ":" + pFragFile;

  init();
}

AntShaderProgram::~AntShaderProgram() {
  takeDown();

  checkedDeleteArray(matrixBuf);
}

void AntShaderProgram::onScreenUp() { init(); }
void AntShaderProgram::onScreenDown() { takeDown(); }

void AntShaderProgram::init() {
  assertGL;
  if (glslOk()) {
    locations.clear();
    attrs.clear();
    vertex = new AntVertexProgram(mVertexFile);
    frag = new AntFragProgram(mFragFile);

    p = glCreateProgramObjectARB();
    assertGL;
    glAttachObjectARB(p, vertex->vertexShader);
    assertGL;
    if (frag->valid())
      glAttachObjectARB(p, frag->fragShader);

    assertGL;
    glLinkProgramARB(p);
    assertGL;
    // FIXME: use PROGRAM_INFO
    //		printInfoLog(p);

    assert(p);
  }
  assertGL;
}

void AntShaderProgram::takeDown() {
  assertGL;
  disable();
  if (glslOk() && !hasQuit())
    glDeleteObjectARB(p);
  checkedDelete(vertex);
  checkedDelete(frag); // checked - no agrubyobject
  locations.clear();
  assertGL;
}

void AntShaderProgram::enable(Renderer *renderer) {
  assertGL;
  if (glslOk()) {
    glUseProgramObjectARB(p);
    on = true;
  }
  assertGL;
}
void AntShaderProgram::disable() {
  assertGL;
  if (glslOk()) {
    glUseProgramObjectARB(0);
    on = false;
  }
  assertGL;
}

void AntShaderProgram::update(Renderer *renderer, float time) {
  if (glslOk()) {
    enable(renderer);
    doUpdate(renderer, time);
    disable();
  }
}

void AntShaderProgram::doUpdate(Renderer * renderer, float time) {}

void AntShaderProgram::sendUniform(const std::string &pName, int i) {
  assertGL;
  glUniform1iARB(getLoc(pName), i);
  assertGL;
}
void AntShaderProgram::sendUniform(const std::string &pName, float f) {
  assertGL;
  glUniform1fARB(getLoc(pName), f);
  assertGL;
}
void AntShaderProgram::sendUniform(const std::string &pName,
                                   const AGVector3 &m) {
  assertGL;
  glUniform3fARB(getLoc(pName), m[0], m[1], m[2]);
  assertGL;
}
void AntShaderProgram::sendUniform(const std::string &pName,
                                   const AGVector4 &m) {
  assertGL;
  glUniform4fARB(getLoc(pName), m[0], m[1], m[2], m[3]);
  assertGL;
}
void AntShaderProgram::sendUniform(const std::string &pName,
                                   const AGMatrix4 &m) {
  assertGL;
  glUniformMatrix4fvARB(getLoc(pName), 1, false, m);
  assertGL;
}
void AntShaderProgram::sendUniform(const std::string &pName,
                                   const std::vector<AGMatrix4> &m) {
  assert(m.size() < 100);
  float *p = matrixBuf;

  for (size_t i = 0; i < m.size(); i++) {
    const float *s = m[i];
    for (size_t j = 0; j < 16; j++)
      *(p++) = *(s++);
  }
  assertGL;
  glUniformMatrix4fvARB(getLoc(pName), m.size(), false, matrixBuf);
  assertGL;
}

GLint AntShaderProgram::getLoc(const std::string &pName) {
  // register only once !

  std::map<std::string, GLint>::iterator i = locations.find(pName);
  if (i != locations.end())
    return i->second;

  assertGL;
  GLint k = glGetUniformLocationARB(p, pName.c_str());
  assertGL;
  locations.insert(std::make_pair(pName, k));
  return k;
}

GLint AntShaderProgram::getAttr(const std::string &pName) {
  assertGL;
  // register only once !
  assert(on);
  std::map<std::string, GLint>::iterator i = attrs.find(pName);
  if (i != attrs.end())
    return i->second;

  assertGL;
  GLint k = glGetAttribLocationARB(p, pName.c_str());
  if (k < 0) {
    cdebug("ERROR:attribute could be get! Maybe it was not defined in the "
           "vertex-shader? :"
           << pName);
  }
  assertGL;
  attrs.insert(std::make_pair(pName, k));
  return k;
}

void AntShaderProgram::sendAttribute(const std::string &pName,
                                     const std::vector<float> &vf) {
  assertGL;
  GLint loc = getAttr(pName);
  assertGL;
  glEnableClientState(GL_VERTEX_ARRAY);
  assertGL;
  glEnableVertexAttribArrayARB(loc); // add array
  assertGL;

  glVertexAttribPointerARB(
      loc, 1, GL_FLOAT, 0, 0,
      &vf[0]); // set attributes (for each vertex an attribute)
  assertGL;
}

///////////////////////////////////////////////////////////////////////////
// AntShadowShader
//
///////////////////////////////////////////////////////////////////////////
AntShadowShader::AntShadowShader(const std::string &pVertexFile,
                                 const std::string &pFragFile)
    : AntShaderProgram(pVertexFile, pFragFile) {}

void AntShadowShader::doUpdate(Renderer *renderer, float time) {
  if (glslOk()) {
    sendUniform("shadowTex", renderer->getShadowUnit());
    sendUniform("normalTex", renderer->getNormalUnit());
    sendUniform("lightComplete", renderer->getCurrentScene()->getLightComplete());
  }
}

void AntShadowShader::enable(Renderer *renderer) {
  assertGL;
  AntShaderProgram::enable(renderer);
  if (glslOk()) {
    Scene *scene = renderer->getCurrentScene();

    glActiveTexture(renderer->getShadowUnit()); // shadow unit

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    // FIXME: move bias to frag-shader, as this shall reduce z-fighting
    float bias[] = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f}; // bias from [-1,
                                                               // 1] to [0, 1]
    glLoadMatrixf(bias);

    glMultMatrixf(scene->getLightProj());
    glMultMatrixf(scene->getLightView());

    glActiveTexture(renderer->getNormalUnit()); // tex unit

    glMatrixMode(GL_MODELVIEW);
  }
  assertGL;
}
void AntShadowShader::disable(Renderer *renderer) {
  assertGL;
  AntShaderProgram::disable();
  if (glslOk()) {
    glActiveTexture(renderer->getShadowUnit()); // shadow unit
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glActiveTexture(renderer->getNormalUnit()); // tex unit
  }
  assertGL;
}

///////////////////////////////////////////////////////////////////////////
// AntWaterShader
//
///////////////////////////////////////////////////////////////////////////

AntWaterShader::AntWaterShader()
    : AntShaderProgram("data/shaders/simplewater.vert",
                       "data/shaders/simplewater.frag") {}
void AntWaterShader::doUpdate(Renderer *renderer, float time) {
  if (glslOk()) {
    AntShaderProgram::doUpdate(renderer, time);

    t += time;
    sendUniform("time", t);
  }
}
