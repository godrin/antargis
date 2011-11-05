/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_messageobject.cc
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

#include "ag_messageobject.h"

#include "rk_debug.h"

#include "ag_main.h"
#include "ag_stringstream.h"
#include "ag_widget.h"

SDL_Event AGEvent::NullEvent={SDL_NOEVENT};

// AGEvent
AGEvent::AGEvent(AGListener *pCaller,const AGString &pName,const SDL_Event &e):mCaller(pCaller),mName(pName),mEvent(e),
mMousePosition(0),mRelMousePosition(0)
      {
        mClipped=false;
      }
AGEvent::~AGEvent() throw()
  {
    if(mMousePosition)
      checkedDelete(mMousePosition);
    if(mRelMousePosition)
      checkedDelete(mRelMousePosition);
  }

bool AGEvent::isMouseEvent() const
{
  return (mEvent.type==SDL_MOUSEMOTION||mEvent.type==SDL_MOUSEBUTTONUP||mEvent.type==SDL_MOUSEBUTTONDOWN);
}


bool AGEvent::isClipped() const
{
  return mClipped;
}
void AGEvent::setClipped(bool f)
  {
    mClipped=f;
  }



void AGEvent::setVector(const AGVector2 &v)
  {
    mVector=v;
  }
/// returns the drag-vector in case of a drag-event
AGVector2 AGEvent::getVector() const
{
  return mVector;
}


/// sets the caller of this event - shouldn't be used outside of AGWidget
void AGEvent::setCaller(AGListener *pCaller)
  {
    mCaller=pCaller;
  }

/// return the name of the event
AGString AGEvent::getName() const
{
  return mName;
}


void AGEvent::setName(const AGString &n)
  {
    mName=n;
  }

bool AGEvent::isSDLEvent() const
{
  return eventOk(mEvent);
}


/// returns the caller of this event
AGListener *AGEvent::getCaller() const
{
  return mCaller;
}

// AGSDLEvent

const SDL_Event &AGEvent::get() const
{
  assert(eventOk(mEvent));
  return mEvent;
}

void AGEvent::setRelMousePosition(const AGVector2 &p)
  {
    if(mRelMousePosition)
      *mRelMousePosition=p;
    else
      mRelMousePosition=new AGVector2(p);
  }

AGVector2 AGEvent::getRelMousePosition() const
{
  if(mRelMousePosition)
    return *mRelMousePosition;
  return getMousePosition();
}


AGVector2 AGEvent::getMousePosition() const
{
  if(mMousePosition)
    return *mMousePosition;

  assert(eventOk(mEvent));
  AGVector2 p;
  switch(mEvent.type) {
  case SDL_MOUSEMOTION:
    p[0]=mEvent.motion.x*getMain()->getVideo()->width()/getMain()->getVideo()->realWidth();
    p[1]=mEvent.motion.y*getMain()->getVideo()->height()/getMain()->getVideo()->realHeight();
    break;

  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEBUTTONDOWN:
    p[0]=mEvent.button.x*getMain()->getVideo()->width()/getMain()->getVideo()->realWidth();
    p[1]=mEvent.button.y*getMain()->getVideo()->height()/getMain()->getVideo()->realHeight();
    break;

  }
  return p;
}

Uint16 AGEvent::getUnicode() const
{
  assert(eventOk(mEvent));
  return mEvent.key.keysym.unicode;
}


SDLKey AGEvent::getKey() const
{
  assert(eventOk(mEvent));
  return mEvent.key.keysym.sym;
}

MSDLMod AGEvent::getMod() const
{
  assert(eventOk(mEvent));
  return mEvent.key.keysym.mod;
}

int AGEvent::getButton() const
{
  assert(eventOk(mEvent));
  switch(mEvent.type) {
  case SDL_MOUSEMOTION:
    return mEvent.motion.state;
    break;

  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEBUTTONDOWN:
    return mEvent.button.button;
    break;

  }
  return 0;
}



// AGMouseEvent

/*
AGMouseEvent::AGMouseEvent(AGListener *pCaller,SDL_Event *pEvent):AGEvent(pCaller)
{
}
AGMouseEvent::~AGMouseEvent()
{
}*/

// AGListener

AGListener::AGListener()
  {
  }

AGListener::~AGListener() throw()
  {
  }

bool AGListener::signal(AGEvent *m)
  {
    return false;
  }

AGCPPListener::~AGCPPListener()
  {
  }



/*
// AGSignal

AGSignal::AGSignal():mCaller(0)
  {
  }

//AGSignal::AGSignal(const AGSignal &s):mListeners(s.mListeners



AGSignal::AGSignal(AGMessageObject *pCaller):mCaller(pCaller)
  {
  }
 */
AGSignal::AGSignal(AGMessageObject *pCaller,const AGString &pName):
  mName(pName),mCaller(pCaller)
        {
        }

AGSignal::~AGSignal()
  {
    std::set<AGListener*>::iterator i=mListeners.begin();
    for(;i!=mListeners.end();i++)
      {
        AGMessageObject *o=dynamic_cast<AGMessageObject*>(*i);
        if(o)
          o->popSignal(this);
      }
  }
#undef connect


/**
   This is the simple connect version. You have to override signal(.) in AGListener and
   provide a reasonable functionality there. For simple usage use AGSignal::connect( AGCPPListener *pListener);

 */
void AGSignal::connect(AGListener &pListener)
  {
    mListeners.insert(&pListener);
    AGMessageObject *o=dynamic_cast<AGMessageObject*>(&pListener);
    if(o)
      o->pushSignal(this);
  }
void AGSignal::disconnect(AGListener &pListener)
  {
    mListeners.erase(&pListener);
  }


/**
   When connecting a signal to a slot you call this function.
   For instance you have a class:
   <pre>
   class A
   {
     public:
     AGSignal sigSomething;
   };
   class B
   {
     public:
     bool eventSomething( AGEvent *e);
   };
   A a;
   B b;
   // you can connect them like this;
   A.sigSomething.connect(slot(&b,&B::eventSomething));
   </pre>
 */

void AGSignal::connect(AGCPPListener *pListener)
  {
    mSimpleListeners.insert(pListener);
  }
void AGSignal::disconnect(AGCPPListener *pListener)
  {
    mSimpleListeners.erase(pListener);
  }

bool AGSignal::signal(AGEvent *m)
  {
    m->setName(mName);
    std::set<AGListener*>::iterator i=mListeners.begin();
    bool value=false;
    for(;i!=mListeners.end();i++)
      {
        if((*i)->signal(m))
          value=true;
      }

    std::set<AGCPPListener*>::iterator j=mSimpleListeners.begin();
    for(;j!=mSimpleListeners.end();j++)
      {
        if((*j)->signal(m))
          value=true;
      }
    // cdebug("value:"<<value);

    return value;
  }

bool AGSignal::operator()(AGEvent *m)
      {
        m->setName(mName);
        return signal(m);
      }

// AGMessageObject


AGMessageObject *AGMessageObject::captureObject=0;


AGMessageObject::AGMessageObject():
  sigActive(this,"sigActive"),
  sigKeyDown(this,"sigKeyDown"),
  sigKeyUp(this,"sigKeyUp"),
  sigMouseMotion(this,"sigMouseMotion"),
  sigMouseButtonDown(this,"sigMouseButtonDown"),
  sigMouseButtonUp(this,"sigMouseButtonUp"),
  sigQuit(this,"sigQuit"),
  sigSysWM(this,"sigSysWM"),
  sigVideoResize(this,"sigVideoResize"),
  mCanReceiveMessages(true)
        {
        }

AGMessageObject::~AGMessageObject() throw()
  {
    std::set<AGSignal*> sigs=mSignals;
    std::set<AGSignal*>::iterator i=sigs.begin();
    for(;i!=sigs.end();i++)
      (*i)->disconnect(*this);
  }

void AGMessageObject::pushSignal(AGSignal *pSignal)
  {
    mSignals.insert(pSignal);
  }

void AGMessageObject::popSignal(AGSignal *pSignal)
  {
    mSignals.erase(pSignal);
  }


//
// exchange with a processEvent(const AGEvent &event)

bool AGMessageObject::processEvent(AGEvent* agEvent)
  {
    bool rc=false;

    if(agEvent->isSDLEvent())
      {
        const SDL_Event &event=agEvent->get();
        switch(event.type) {
        case SDL_ACTIVEEVENT:
          rc = eventActive(agEvent) || sigActive(agEvent);
          break;

        case SDL_KEYDOWN:
          rc = eventKeyDown(agEvent) || sigKeyDown(agEvent);
          break;

        case SDL_KEYUP:
          rc = eventKeyUp(agEvent) || sigKeyUp(agEvent);
          break;

        case SDL_MOUSEMOTION:
          rc = eventMouseMotion(agEvent) || sigMouseMotion(agEvent);
          break;

        case SDL_MOUSEBUTTONDOWN:
          rc = eventMouseButtonDown(agEvent) || sigMouseButtonDown(agEvent);
          break;

        case SDL_MOUSEBUTTONUP:
          rc = eventMouseButtonUp(agEvent) || sigMouseButtonUp(agEvent);
          break;

        case SDL_QUIT:
          rc = eventQuit(agEvent) || sigQuit(agEvent);
          break;

        case SDL_SYSWMEVENT:
          rc = eventSysWM(agEvent) || sigSysWM(agEvent);
          break;

        case SDL_VIDEORESIZE:
          rc = eventResize(agEvent) || sigVideoResize(agEvent);
          break;

        default:
          rc = false;
          break;
        }
      }

    if(rc)
      {
        AGWidget *w=dynamic_cast<AGWidget*>(this);
        if(w)
          cdebug(toString(&agEvent->get())<<" eaten up by "<<typeid(*this).name()<<" "<<w->getName());
        else
          cdebug(toString(&agEvent->get())<<" eaten up by "<<typeid(*this).name());//<<" "<<getName());
      }

    return rc;
  }



bool AGMessageObject::eventActive(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventKeyDown(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventKeyUp(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventMouseMotion(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventMouseButtonDown(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventMouseButtonUp(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventQuit(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventQuitModal(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventSysWM(AGEvent *m)
  {
    return false;
  }
bool AGMessageObject::eventResize(AGEvent *m)
  {
    return false;
  }

Uint8 AGMessageObject::getButtonState() const
{
  int x,y;
  Uint8 b=SDL_GetMouseState(&x,&y);
  return b;
}
AGVector2 AGMessageObject::getMousePosition() const
{
  int x,y;
  SDL_GetMouseState(&x,&y);
  return AGVector2(x,y);
}


bool AGMessageObject::acceptEvent(const SDL_Event *pEvent)
  {
    return true;
  }

AGEvent *newEvent(AGListener *pCaller,const AGString &pName,const SDL_Event &s)
  {
    return new AGEvent(pCaller,pName,s);
  }


AGString toString(SDL_keysym k)
  {
    AGStringStream os;
    os<<k.scancode<<":"<<(int)k.sym<<":"<<k.mod<<":"<<k.unicode;

    return os.str();
  }

AGString getUntil(AGString &b,const AGString &f)
  {
    size_t i=b.find(f);
    AGString s=b.substr(0,i);
    if(i!=AGString::npos)
      b=b.substr(i+1,AGString::npos);
    return s;
  }

SDL_keysym toKeysym(const AGString &s)
  {
    SDL_keysym k;
    AGString b=s;
    k.scancode=getUntil(b,":").toUint8();
    k.sym=(SDLKey)getUntil(b,":").toUint8();
    k.mod=(MSDLMod)getUntil(b,":").toUint8();
    k.unicode=getUntil(b,":").toSint16();


    return k;
  }

AGString toString(const SDL_Event *pEvent)
  {
    AGStringStream os;
    if(pEvent)
      {
        switch(pEvent->type)
          {
        case SDL_ACTIVEEVENT:
          os<<"SDL_ACTIVEEVENT:"<<(int)pEvent->active.gain<<":"<<(int)pEvent->active.state;
          break;
        case SDL_KEYDOWN:
          os<<"SDL_KEYDOWN:"<<(int)pEvent->key.which<<":"<<(int)pEvent->key.state<<":"<<toString(pEvent->key.keysym);
          break;
        case SDL_KEYUP:
          os<<"SDL_KEYUP:"<<(int)pEvent->key.which<<":"<<(int)pEvent->key.state<<":"<<toString(pEvent->key.keysym);
          break;
        case SDL_MOUSEMOTION:
          os<<"SDL_MOUSEMOTION:"<<(int)pEvent->motion.which<<":"<<(int)pEvent->motion.state<<":"<<pEvent->motion.x<<":"<<pEvent->motion.y<<":"<<pEvent->motion.xrel<<":"<<pEvent->motion.yrel;
          break;
        case SDL_MOUSEBUTTONDOWN:
          os<<"SDL_MOUSEBUTTONDOWN:"<<(int)pEvent->button.which<<":"<<(int)pEvent->button.button<<":"<<(int)pEvent->button.state<<":"<<pEvent->button.x<<":"<<pEvent->button.y;
          break;
        case SDL_MOUSEBUTTONUP:
          os<<"SDL_MOUSEBUTTONUP:"<<(int)pEvent->button.which<<":"<<(int)pEvent->button.button<<":"<<(int)pEvent->button.state<<":"<<pEvent->button.x<<":"<<pEvent->button.y;
          break;
        case SDL_JOYAXISMOTION:
          os<<"SDL_JOYAXISMOTION:"<<(int)pEvent->jaxis.which<<":"<<(int)pEvent->jaxis.axis<<":"<<pEvent->jaxis.value;
          break;
        case SDL_JOYBALLMOTION:
          os<<"SDL_JOYBALLMOTION:"<<(int)pEvent->jball.which<<":"<<(int)pEvent->jball.ball<<":"<<pEvent->jball.xrel<<":"<<pEvent->jball.yrel;
          break;
        case SDL_JOYHATMOTION:
          os<<"SDL_JOYHATMOTION:"<<(int)pEvent->jhat.which<<":"<<(int)pEvent->jhat.hat<<":"<<(int)pEvent->jhat.value;
          break;
        case SDL_JOYBUTTONDOWN:
          os<<"SDL_JOYBUTTONDOWN:"<<(int)pEvent->jbutton.which<<":"<<(int)pEvent->jbutton.button<<":"<<(int)pEvent->jbutton.state;
          break;
        case SDL_JOYBUTTONUP:
          os<<"SDL_JOYBUTTONUP:"<<(int)pEvent->jbutton.which<<":"<<(int)pEvent->jbutton.button<<":"<<(int)pEvent->jbutton.state;
          break;
        case SDL_QUIT:
          os<<"SDL_QUIT:";
          break;
        case SDL_SYSWMEVENT:
          os<<"SDL_DUMMY;";
          break;
        case SDL_VIDEOEXPOSE:
          os<<"SDL_VIDEOEXPOSE";
          break;
        case SDL_NOEVENT:
          os<<"SDL_NOEVENT";
          break;
        default:
          dbout(1,"UNKNOWN SDL_EVENT:"<<pEvent->type);
          os<<"SDL_NOEVENT";
          break;
          }
        if(os.str().length())
          return os.str();

      }
    return "SDL_NOEVENT";
  }



SDL_Event *toSDLEvent(const AGString &p)
  {
    AGString b=p;
    AGString t=getUntil(b,":");
    static SDL_Event event;

    event.type=0;

    if(t=="SDL_ACTIVEEVENT")
      {
        event.type=SDL_ACTIVEEVENT;
        event.active.gain=getUntil(b,":").toUint8();
        event.active.state=getUntil(b,":").toUint8();
      }
    else if(t=="SDL_KEYDOWN")
      {
        event.type=SDL_KEYDOWN;
        event.key.which=getUntil(b,":").toUint8();
        event.key.state=getUntil(b,":").toUint8();
        event.key.keysym=toKeysym(b);
      }
    else if(t=="SDL_KEYUP")
      {
        event.type=SDL_KEYUP;
        event.key.which=getUntil(b,":").toUint8();
        event.key.state=getUntil(b,":").toUint8();
        event.key.keysym=toKeysym(b);

      }
    else if(t=="SDL_MOUSEMOTION")
      {
        event.type=SDL_MOUSEMOTION;
        event.motion.which=getUntil(b,":").toUint8();
        event.motion.state=getUntil(b,":").toUint8();
        event.motion.x=getUntil(b,":").toSint16();
        event.motion.y=getUntil(b,":").toSint16();
        event.motion.xrel=getUntil(b,":").toSint16();
        event.motion.yrel=getUntil(b,":").toSint16();
      }
    else if(t=="SDL_MOUSEBUTTONDOWN")
      {
        event.type=SDL_MOUSEBUTTONDOWN;
        event.button.which=getUntil(b,":").toUint8();
        event.button.button=getUntil(b,":").toUint8();
        event.button.state=getUntil(b,":").toUint8();
        event.button.x=getUntil(b,":").toSint16();
        event.button.y=getUntil(b,":").toSint16();
      }
    else if(t=="SDL_MOUSEBUTTONUP")
      {
        event.type=SDL_MOUSEBUTTONUP;
        event.button.which=getUntil(b,":").toUint8();
        event.button.button=getUntil(b,":").toUint8();
        event.button.state=getUntil(b,":").toUint8();
        event.button.x=getUntil(b,":").toSint16();
        event.button.y=getUntil(b,":").toSint16();
      }
    else if(t=="SDL_JOYAXISMOTION")
      {
        event.type=SDL_JOYAXISMOTION;
        event.jaxis.which=getUntil(b,":").toUint8();
        event.jaxis.axis=getUntil(b,":").toUint8();
        event.jaxis.value=getUntil(b,":").toUint8();
      }
    else if(t=="SDL_JOYBALLMOTION")
      {
        event.type=SDL_JOYBALLMOTION;
        event.jball.which=getUntil(b,":").toUint8();
        event.jball.ball=getUntil(b,":").toUint8();
        event.jball.xrel=getUntil(b,":").toSint16();
        event.jball.yrel=getUntil(b,":").toSint16();
      }
    else if(t=="SDL_JOYHATMOTION")
      {
        event.type=SDL_JOYHATMOTION;
        event.jhat.which=getUntil(b,":").toUint8();
        event.jhat.hat=getUntil(b,":").toUint8();
        event.jhat.value=getUntil(b,":").toUint8();
      }
    else if(t=="SDL_JOYBUTTONDOWN")
      {
        event.type=SDL_JOYBUTTONDOWN;
        event.jbutton.which=getUntil(b,":").toUint8();
        event.jbutton.button=getUntil(b,":").toUint8();
        event.jbutton.state=getUntil(b,":").toUint8();
      }
    else if(t=="SDL_JOYBUTTONUP")
      {
        event.type=SDL_JOYBUTTONUP;
        event.jbutton.which=getUntil(b,":").toUint8();
        event.jbutton.button=getUntil(b,":").toUint8();
        event.jbutton.state=getUntil(b,":").toUint8();
      }
    else if(t=="SDL_SYSWMEVENT")
      {
        event.type=SDL_SYSWMEVENT;
        // FIXME
      }
    else if(t=="SDL_VIDEOEXPOSE")
      {
        event.type=SDL_VIDEOEXPOSE;

      }
    else if(t=="SDL_DUMMY")
      {
        event.type=SDL_USEREVENT;
      }
    else if(t=="SDL_QUIT")
      {
        event.type=SDL_QUIT;
      }
    else
      {
        event.type=SDL_NOEVENT;
        return &event;
      }
    if(event.type)
      return &event;

    return 0;
  }
bool eventOk(const SDL_Event &pEvent)
  {
    //  dbout(1,"eventOk: check "<<(int)pEvent.type<<"!="<<SDL_NOEVENT<<" ???");
    return (int)pEvent.type!=SDL_NOEVENT;
  }

void resetEvent(SDL_Event &pEvent)
  {
    pEvent.type=SDL_NOEVENT;
  }
