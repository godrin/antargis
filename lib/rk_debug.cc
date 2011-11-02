/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_debug.cc
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

#include "rk_debug.h"

#include <fstream>
#include <iostream>

#ifdef __WIN32__
#include <winsock2.h>
#endif
#include <ruby.h>
#include <SDL.h>

bool quietLog=false;
void setQuiet()
  {
    quietLog=true;
  }

static bool gRubyRaising=false;
void agRaise(const std::string &s)
  {
    cdebug("assertion failed:"<<s);
    if(gRubyRaising)
      rb_raise(rb_eRuntimeError,s.c_str(),"");
    else
      throw std::runtime_error(s);
  }

void setRubyRaising(bool flag)
  {
    gRubyRaising=flag;
  }

size_t gDebugLevel=0;

size_t getDebugLevel()
  {
    return gDebugLevel;
  }
void setDebugLevel(size_t t)
  {
    gDebugLevel=t;
  }


#ifndef MNDEBUG
int D::d=0;

std::ofstream debugOFS("debug.txt");

std::ostream &getDebug()
  {
    if(quietLog)
      return debugOFS;
    else
      return std::cout;
  }

size_t gDebugIndex=0;

size_t getDebugIndex()
  {
    return gDebugIndex;
  }



D::D(std::string c):
  m(c)
  {
    indent();
    gDebugIndex++;

    debugout_checked(2,"start of:"<<c<<"("<<gDebugIndex<<")"<<std::endl);
    d++;
  }
AGEXPORT D::~D()
  {
    d--;
    indent();
    debugout_checked(2,"end   of:"<<m<<std::endl);
  }
void D::indent()
  {
    for(int i=0;i<d;i++)
      debugout_checked(2,"  ");
  }


#endif

#ifdef __APPLE__

//#include <mach-o/nlist.h>

#include "execinfo.h"
void printStacktrace()
  {
    void* callstack[128];
    int i, frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    printf("FRAMES:%d\n",frames);
    for (i = 0; i < frames; ++i) {
      printf("%s\n", strs[i]);
    }
    free(strs);
    /*
    struct nlist *nl;
    int result=nlist("antargis.bundle",nl);
    std::cout<<"RESULT:"<<result<<std::endl;
*/
  }

#else

void printStacktrace()
  {
  }

#endif

