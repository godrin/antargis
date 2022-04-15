#include <ag_video.h>

#include <ag_glscreen.h>
#include <ag_gltexture.h>
#include <ag_main.h>
#include <ag_screen.h>
#include <ag_sdlscreen.h>
#include <ag_surfacemanager.h>
#include <ag_vdebug.h>
#include <rk_debug.h>

#include <SDL_image.h>

static int lastWidth = 0;
static int lastHeight = 0;
static int lastVWidth = 0;
static int lastVHeight = 0;
static int lastDepth = 0;
static bool fullScreen = false;
static bool lastGL = false;

static const SDL_VideoInfo *videoInfo;

AGVideoManager::AGVideoManager() { mScreen = 0; }

void AGVideoManager::flip() {
  assert(mScreen);
  mScreen->flip();
}

/**
   initializes video mode
   @param pw screen-width
   @param ph screen height.
   @param pd color depth in bits - typically 32
   @param fs bool fullscreen. true for fullscreen, false for window
   @param opengl use opengl or not
 */
void AGVideoManager::initVideo(int w, int h, int d, bool fs, bool gl, int vw,
                               int vh) {
  if (mScreen) {
    getSurfaceManager()->clear();
    getSurfaceManager()->clear();

    AGGLScreen *ms = dynamic_cast<AGGLScreen *>(mScreen);
    if (ms)
      ms->screenDown();
  }

  lastGL = gl;
  lastWidth = w;
  lastHeight = h;
  lastDepth = d;
  fullScreen = fs;

  if (!videoInfo) {
    videoInfo = SDL_GetVideoInfo();
    if (!videoInfo) {
      std::cerr << "SDL could not get video-info" << std::endl;
      exit(1);
    }
  }
  int videoFlags = 0;

  if (gl)
    videoFlags |= SDL_OPENGL;
  else
    videoFlags |= SDL_SWSURFACE;

  if (fs)
    videoFlags |= SDL_FULLSCREEN;

  if (gl)
    videoFlags |= SDL_DOUBLEBUF;

  videoFlags |= SDL_RESIZABLE;

  SDL_Surface *ms =
      SDL_SetVideoMode(w, h, videoInfo->vfmt->BitsPerPixel, videoFlags);
  if (!ms) {
    std::cerr << "Initing video mode failed!" << std::endl;
    std::cerr << "SDL:Error:" << SDL_GetError() << std::endl;
    std::cerr << "If you experience errors here, it may be that one following "
                 "is true:"
              << std::endl;
    std::cerr << "* SDL is compiled without OpenGL support" << std::endl;
    std::cerr << "* Your display has no support for GLX" << std::endl;
    std::cerr << "* You're running SDL with framebuffer - instead of X"
              << std::endl;
    exit(1);
  }

  if (mScreen) {
    checkedDelete(mScreen);
  }

  if (vw < w)
    vw = w;
  if (vh < h)
    vh = h;

  lastVWidth = vw;
  lastVHeight = vh;

  if (gl) {
    AGGLScreen *ms = new AGGLScreen(w, h, vw, vh);
    ms->screenUp();

    setScreen(mScreen = ms);
  } else
    setScreen(mScreen = new AGSDLScreen(ms));

  SDL_WM_SetCaption("Antargis", "Antargis");
}

void AGVideoManager::toggleFull() {
  initVideo(lastWidth, lastHeight, lastDepth, !fullScreen, lastGL, lastVWidth,
            lastVHeight);
}

void AGVideoManager::setIcon(const std::string &pFile) {
  std::string file = loadFile(pFile);

  if (file.length() == 0)
    cdebug("file :" << pFile << " possibly not found!");

  SDL_Surface *s = IMG_Load_RW(
      SDL_RWFromMem(const_cast<char *>(file.c_str()), file.length()), false);
  SDL_WM_SetIcon(s, 0);
  assertGL;
}

void AGVideoManager::setCaption(const std::string &pCaption) {
  SDL_WM_SetCaption(pCaption.c_str(), 0);
}

bool AGVideoManager::fullscreen() const { return fullScreen; }

int AGVideoManager::realWidth() const { return lastWidth; }
int AGVideoManager::realHeight() const { return lastHeight; }

int AGVideoManager::width() const { return lastVWidth; }
int AGVideoManager::height() const { return lastVHeight; }

AGVideoManager *getVideo() {
  AGVideoManager *m = dynamic_cast<AGVideoManager *>(getMain()->getVideo());
  assert(m);
  return m;
}
