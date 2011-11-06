/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_glsl.h
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef AG_GLSL_H
#define AG_GLSL_H

#include "ag_gl.h"
#include "rk_tools.h"
#include "ag_geometry.h"

#include <map>

class Scene;

class AGEXPORT AGVertexProgram
{
 public:
  AGVertexProgram(const std::string &pFile);
  ~AGVertexProgram();

 private:
  GLhandleARB vertexShader;

  friend class AGShaderProgram;
};

class AGEXPORT AGFragProgram
{
 public:
  AGFragProgram(const std::string &pFile);
  AGFragProgram();
  ~AGFragProgram();

  bool valid() const;

 private:
  GLhandleARB fragShader;
  bool mValid;

  friend class AGShaderProgram;
};


class AGEXPORT AGShaderParameter
{
 public:

};

class AGEXPORT AGShaderProgram
{
  AGVertexProgram vertex;
  AGFragProgram frag;
  std::map<std::string,GLint> locations;
  std::map<std::string,GLint> attrs;
  float *matrixBuf;
  bool on;
  std::string name;
 protected:
  GLhandleARB p;

 public:
  AGShaderProgram(const std::string &pVertexFile,const std::string &pFragFile);
  virtual ~AGShaderProgram();
  virtual void enable();
  virtual void disable();

  void update(float time);
  virtual void doUpdate(float time);

  GLint getLoc(const std::string &pName);
  GLint getAttr(const std::string &pName);

  void sendUniform(const std::string &pName,int i);
  void sendUniform(const std::string &pName,float f);
  void sendUniform(const std::string &pName,const AGVector3 &m);
  void sendUniform(const std::string &pName,const AGVector4 &m);
  void sendUniform(const std::string &pName,const AGMatrix4 &m);
  void sendUniform(const std::string &pName,const std::vector<AGMatrix4> &m);

  void sendAttribute(const std::string &pName,const std::vector<float> &vf);
};

AGEXPORT bool glslOk();

#endif
