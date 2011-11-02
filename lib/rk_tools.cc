/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * ag_tools.cc
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

#include "rk_tools.h"
#include "rk_debug.h"

#include <sstream>

int toInt(const std::string &s)
  {
    return atoi(s.c_str());
  }
long toLong(const std::string &s)
  {
    return atol(s.c_str());
  }
float toFloat(const std::string &s)
  {
    float f=atof(s.c_str());
    return f;
  }

int fromHex(const std::string &s)
  {
    int i=0;
    for(size_t j=0;j<s.length();j++)
      {
        int k=0;
        i<<=4;
        char c=s[j];
        if(c>='A'&&c<='F')
          k=c-'A'+10;
        else if(c>='a' && c<='f')
          k=c-'a'+10;
        else if(c>='0' && c<='9')
          k=c-'0';
        i+=k;
      }
    return i;
  }
std::string toHex(int i)
  {
    std::string s;
    std::string c="0";
    while(i!=0)
      {
        int k=i&0xF;

        if(k>9)
          c[0]='A'+k-10;
        else
          c[0]='0'+k;

        s=c+s;
        i>>=4;
      }


    while(s.length()<2)
      s=std::string("0")+s;
    return s;
  }


std::vector<std::string> split(const std::string &needle,const std::string &haystack)
{
  std::vector<std::string> d;
  size_t i,last=0;
  for(i=haystack.find(needle);i!=std::string::npos;i=haystack.find(needle,i+1))
    {
      d.push_back(haystack.substr(last,i-last));
      last=i+needle.length();
    }
  i=haystack.length();
  d.push_back(haystack.substr(last,i-last));
  return d;
}



std::string replace(const std::string &s,const std::string &a,const std::string &b)
  {
    std::string str=s;
    size_t i=str.find(a);
    while(i!=str.npos)
      {
        str=str.substr(0,i)+b+str.substr(i+a.length(),str.length()-i-a.length());
        i=str.find(a,i+b.length());
      }
    return str;
  }
