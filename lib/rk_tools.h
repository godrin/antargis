/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_tools.h
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

#ifndef AG_TOOLS_H
#define AG_TOOLS_H

#include <string>
#include <list>
#include <sstream>
#include <vector>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include <rk_base.h>

std::vector<std::string> AGEXPORT split(const std::string &n,const std::string &h);


std::string AGEXPORT replace(const std::string &s,const std::string &a,const std::string &b);

template<class T>
inline std::string toString(const T&t)
{
  std::ostringstream os;
  os<<t;
  return os.str();
}

template<>
inline std::string toString(const bool&b)
{
  if(b)
    return "true";
  else
    return "false";
}

inline bool toBool(const std::string &s)
{
  return(s=="true");
}

AGEXPORT int toInt(const std::string &s);
AGEXPORT long toLong(const std::string &s);
AGEXPORT float toFloat(const std::string &s);
AGEXPORT int fromHex(const std::string &s);
AGEXPORT std::string toHex(int i);

template<class T>
void append(std::list<T> &l1,const std::list<T> &l2)
{
  typename std::list<T>::const_iterator i=l2.begin();
  for(;i!=l2.end();i++)
    l1.push_back(*i);
}

template<class T>
void append(std::vector<T> &l1,const std::vector<T> &l2)
{
  typename std::vector<T>::const_iterator i=l2.begin();
  for(;i!=l2.end();i++)
    l1.push_back(*i);
}

#define AGsign(x) ((x)<0?-1:(x)>0?1:0)

#endif
