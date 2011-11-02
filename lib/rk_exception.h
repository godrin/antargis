/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_exception.h
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

#ifndef __ANTARGISGUI_EXCEPTION_H__
#define __ANTARGISGUI_EXCEPTION_H__

#include <rk_base.h>
#include <stdexcept>

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

class AGEXPORT AGException:public std::exception
{
public:
  AGException(const std::string &e):s(e)
  {
  }

  virtual ~AGException() throw ()
  {
  }

  const char*what() const throw()
  {
    return s.c_str();
  }

private:
  std::string s;
};

#endif
