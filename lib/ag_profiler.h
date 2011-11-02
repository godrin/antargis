/*
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

#ifndef AG_PROFILER_H
#define AG_PROFILER_H

#include <rk_base.h>

#include <string>
#include <SDL.h>

#if CPU_FAMILY == PPC
#ifdef MPROFILE
#undef MRPOFILE
#endif
#endif


#ifdef MPROFILE

#define Uint64 long long
#define Sint64 signed long long

struct SOURCE_LINE
{
  Uint64 time;       ///< Total time spent in function
  Uint64 count;      ///< Total function calls
  int line;          ///< __LINE__
  const char *file;  ///< __FILE__
  const char *funct; ///< __FUNCTION__
  const char *name;  ///< NULL or a descriptor string
};

/// \brief Call stack tracker
///
/// Object of this class is created on stack in STACKTRACE macro, so it's creating/destruction can
/// be used to manage  call stack.
class AGEXPORT UserStackTraceHelper
{
 public:
  UserStackTraceHelper( SOURCE_LINE* srcline);
  ~UserStackTraceHelper();

  ///  get stack trace string
    static std::string getStackTraceString();
    static std::string getProfilerString();
};


#define STACKTRACE static SOURCE_LINE _srcline = { 0, 0, __LINE__, __FILE__, __FUNCTION__, 0 }; UserStackTraceHelper _stacktrace_ ( &_srcline );
#define _STACKTRACE(A) static SOURCE_LINE _srcline = {0, 0, __LINE__, __FILE__, __FUNCTION__, A }; UserStackTraceHelper _stacktrace_ ( &_srcline );


#else // MPROFILE
#define STACKTRACE

#endif // MPROFILE

#endif
