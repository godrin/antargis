#include "rk_utf8.h"
#include "rk_debug.h"
#include <string>

#include <SDL.h>

size_t agFirstCharSize(const std::string &s,size_t pos)
{
  unsigned char c=s[pos];
  size_t l=1;
  if((c&0xF0)==0xF0)
    l=4;
  else if((c&0xE0)==0xE0)
    l=3;
  else if((c&0xC0)==0xC0)
    l=2;
  assert(l+pos<=s.length());
  return l;
    
}
size_t agStringLength(const std::string &s)
{
  size_t c=0;
  size_t l=0;
  //  cdebug("in:"<<s);
  while(c<s.length())
    {
      size_t cl=agFirstCharSize(s,c);
      //      cdebug(cl<<"::"<<c<<"  "<<s.length());
      c+=cl;
      l++;
    }
  //  cdebug("len:"<<l);
  return l;
}

std::string agSubStr(const std::string &s,size_t from,size_t len)
{
  size_t i=0;
  size_t c=0;
  while(i<from && c<s.length())
    {
      c+=agFirstCharSize(s,c);
      i++;
    }
  if(i<from)
    return "";

  std::string r;
  size_t d;
  for(i=0;i<len && c<s.length();i++)
    {
      d=agFirstCharSize(s,c);
      r+=s.substr(c,d);
      c+=d;
    }
  return r;
}

std::string unicode2Utf8(Uint16 unicode)
{
  std::ostringstream os;

  if(unicode < 0x80)
    os<<char(unicode & 0x7f);
  else if(unicode < 0x800)
    os<<char((unicode >>6)|0xc0)<<char((unicode & 0x3f)|0x80);
#ifdef BIGGER_UNICODE
  else if(unicode < 0x10000)
    os<<char((unicode >>12)|0xe0)<<char(((unicode >> 6)&0x3f)|0x80)<<char((unicode&0x3f)|0x80);
  else if(unicode <= 0x0010 FFFF)
    os<<char((unicode >>18)|0xF0)<<char(((unicode >>12)&0x3f)|0x80)<<char(((unicode>>6)0x3f)|0x80)<<char((unicode&0x3f)|0x80);
#else
  else
    os<<char((unicode >>12)|0xe0)<<char(((unicode >> 6)&0x3f)|0x80)<<char((unicode&0x3f)|0x80);
#endif
  return os.str();
}
