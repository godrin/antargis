#include "ag_gl.h"
#include "ag_glsl.h"
#include "ag_vdebug.h"
#include "ag_main.h"

std::map<std::string,AGVertexProgram*> mVertexPrograms;
std::map<std::string,AGFragProgram*> mFragPrograms;

AGVertexProgram *getVertexProgram(const std::string &pFile)
{
	std::map<std::string,AGVertexProgram*>::iterator i=mVertexPrograms.find(pFile);

	if(i==mVertexPrograms.end())
	{
		AGVertexProgram *p=new AGVertexProgram(pFile);
		mVertexPrograms[pFile]=p;
		return p;
	}
	return i->second;
}

AGFragProgram *getFragProgram(const std::string &pFile)
{
	std::map<std::string,AGFragProgram*>::iterator i=mFragPrograms.find(pFile);

	if(i==mFragPrograms.end())
	{
		AGFragProgram *p=new AGFragProgram(pFile);
		mFragPrograms[pFile]=p;
		return p;
	}
	return i->second;
}

int GLSL_ok=-1;
bool glslOk()
{
	//  return false;
	// do not check in each call, because this is slow!!!
	if(GLSL_ok<0)
	{
		//GLeeInit();
		GLSL_ok=(GL_EXT_vertex_shader && GL_ARB_fragment_shader && GL_ARB_shading_language_100);
	}

	return GLSL_ok;
}

void printInfoLog(GLhandleARB obj) {
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	GLint compiled = 0;
	assertGL;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		assertGL;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,
				&infologLength);
		assertGL;

		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			if(infoLog && charsWritten>0) {
				cdebug("GLSL Error:"<<infoLog);
			}
			free(infoLog);
		}
	}
	assertGL;
}


AGVertexProgram::AGVertexProgram(const std::string &pFile)
{
	assertGL;
	if(glslOk())
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string s=loadFile(pFile);

		const char*a=s.c_str();
		const GLchar**x=&a;
		assertGL;
		glShaderSource(vertexShader, 1, x,NULL);
		assertGL;
		glCompileShader(vertexShader);
		assertGL;
		printInfoLog(vertexShader);
		assertGL;
	}
	assertGL;
}

AGVertexProgram::~AGVertexProgram()
{
	assertGL;
	if(glslOk() && !hasQuit())
		glDeleteShader(vertexShader);
	assertGL;
}

AGFragProgram::AGFragProgram(const std::string &pFile)
{
	assertGL;
	mValid=false;
	//  CTRACE;
	if(glslOk() && pFile.length()>0)
	{
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		assertGL;
		std::string s=loadFile(pFile);
		const char*a=s.c_str();
		const GLchar**x=&a;
		glShaderSource(fragShader, 1, x,NULL);
		assertGL;
		glCompileShader(fragShader);
		assertGL;
		printInfoLog(fragShader);
		mValid=true;
		assertGL;

	}
	assertGL;
}

AGFragProgram::AGFragProgram()
{
	mValid=false;
}

AGFragProgram::~AGFragProgram()
{
	assertGL;
	CTRACE;
	if(glslOk() && !hasQuit())
		glDeleteShader(fragShader);
	assertGL;

}


bool AGFragProgram::valid() const
{
	return mValid;
}


AGShaderProgram::AGShaderProgram(const std::string &pVertexFile,const std::string &pFragFile):
	//  vertex(getVertexProgram(pVertexFile)),frag(getFragProgram(pFragFile))
	vertex(pVertexFile),frag(pFragFile)
{
	assertGL;
	//  CTRACE;
	if(glslOk())
	{
		p = glCreateProgram();
		glAttachShader(p,vertex.vertexShader);
		if(frag.valid())
			glAttachShader(p,frag.fragShader);

		assertGL;
		glLinkProgram(p);
		assertGL;
		printInfoLog(p);

		assert(p);
	}
	on=false;
	matrixBuf=new float[16*100];
	name=pVertexFile+":"+pFragFile;
	assertGL;

}

AGShaderProgram::~AGShaderProgram()
{
	assertGL;
	disable();
	CTRACE;
	cdebug("name:"<<name);
	if(glslOk() && !hasQuit())
		glDeleteShader(p);
	cdebug("name:"<<name);
	checkedDeleteArray(matrixBuf); // checked - no agrubyobject
	cdebug("name:"<<name);
	assertGL;

}


void AGShaderProgram::enable()
{
	if(glslOk())
	{
		assertGL;
		glUseProgram(p);
		on=true;
		assertGL;

	}
}
void AGShaderProgram::disable()
{
	if(glslOk())
	{
		assertGL;
		glUseProgram(0);
		on=false;
		assertGL;
	}
}

void AGShaderProgram::update(float time)
{
	if(glslOk())
	{
		enable();
		doUpdate(time);
		disable();
		assertGL;
	}
}

void AGShaderProgram::doUpdate(float time)
{
}

void AGShaderProgram::sendUniform(const std::string &pName,int i)
{
	assertGL;
	glUniform1i(getLoc(pName),i);
	assertGL;
}
void AGShaderProgram::sendUniform(const std::string &pName,float f)
{
	assertGL;
	glUniform1f(getLoc(pName),f);
	assertGL;
}
void AGShaderProgram::sendUniform(const std::string &pName,const AGVector3 &m)
{
	assertGL;
	glUniform3f(getLoc(pName),m[0],m[1],m[2]);
	assertGL;
}
void AGShaderProgram::sendUniform(const std::string &pName,const AGVector4 &m)
{
	assertGL;
	glUniform4f(getLoc(pName),m[0],m[1],m[2],m[3]);
	assertGL;
}
void AGShaderProgram::sendUniform(const std::string &pName,const AGMatrix4 &m)
{
	assertGL;
	glUniformMatrix4fv(getLoc(pName),1,false,m);
	assertGL;
}
void AGShaderProgram::sendUniform(const std::string &pName,const std::vector<AGMatrix4> &m)
{
	assert(m.size()<100);
	float *p=matrixBuf;

	for(size_t i=0;i<m.size();i++)
	{
		const float *s=m[i];
		for(size_t j=0;j<16;j++)
			*(p++)=*(s++);
	}
	assertGL;
	glUniformMatrix4fv(getLoc(pName),m.size(),false,matrixBuf);
	assertGL;
}

GLint AGShaderProgram::getLoc(const std::string &pName)
{
	// register only once !

	assertGL;
	std::map<std::string,GLint>::iterator i=locations.find(pName);
	if(i!=locations.end())
		return i->second;

	GLint k=glGetUniformLocation(p,pName.c_str());
	locations.insert(std::make_pair(pName,k));
	assertGL;
	return k;
}

GLint AGShaderProgram::getAttr(const std::string &pName)
{
	assertGL;
	//  TRACE;
	// register only once !
	assert(on);
	std::map<std::string,GLint>::iterator i=attrs.find(pName);
	if(i!=attrs.end())
		return i->second;

	assertGL;
	GLint k=glGetAttribLocation(p,pName.c_str());
	//  cdebug("k:"<<k);
	if(k<0)
	{
		cdebug("ERROR:attribute could be get! Maybe it was not defined in the vertex-shader? :"<<pName);
	}
	assertGL;
	attrs.insert(std::make_pair(pName,k));
	assertGL;
	return k;
}

void AGShaderProgram::sendAttribute(const std::string &pName,const std::vector<float> &vf)
{
	assertGL;
	GLint loc=getAttr(pName);
	assertGL;
	glEnableClientState(GL_VERTEX_ARRAY);
	assertGL;
	glEnableVertexAttribArray(loc); // add array
	assertGL;

	glVertexAttribPointer(loc,1,GL_FLOAT,0,0,&vf[0]); // set attributes (for each vertex an attribute)
	assertGL;
}

