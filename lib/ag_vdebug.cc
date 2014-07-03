#include <ag_vdebug.h>
#include <ag_video.h>

#include <SDL_opengl.h>
#include <SDL_error.h>
#include <GL/gl.h>

void agAssertGL(std::string s)
  {
    if(opengl())
      {
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
          std::ostringstream msg;
          msg <<s<< ": OpenGLError "
          << gluErrorString(error);

          std::cerr<<msg.str()<<std::endl;
          throw std::runtime_error(msg.str());
        }
      }

    // asserting SDL_errors
    char *se=SDL_GetError();
    if(se)
      {
        if(strlen(se)>0)
          {
            std::cerr<<"SDL_Error:"<<s<<":"<<se<<std::endl;
            if(std::string(se).substr(0,37)=="Failed loading glXGetSwapIntervalMESA" ||
                std::string(se).substr(0,40)=="DirectSoundCreate: No audio device found" ||
                std::string(se).substr(0,30)=="Failed to access the SoundFont" 
                )
              std::cerr<<"IGNORING THIS ERROR!"<<std::endl;
            else
              throw std::runtime_error(se);
            SDL_ClearError();
          }
      }
    else
      {
        std::cerr<<"MUH"<<std::endl;
      }

  }


