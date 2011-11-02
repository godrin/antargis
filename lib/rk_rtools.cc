#include <rk_rtools.h>

#include <ruby.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

std::string rubyHash(const std::string &p)
{
  rb_eval_string("require 'digest/md5'");
  VALUE l=rb_eval_string("Digest::MD5");
  VALUE r=rb_funcall(l,rb_intern("digest"),1,rb_str_new(p.c_str(),p.length()));

  std::string s(RSTRING_PTR(r), RSTRING_LEN(r));

  return binaryToHex(s,false);
  
}

std::string rubyHashOld(const std::string &p)
{
  // FIXME: TRY USING ruby's Digest::MD5::digest("xy")
  rb_eval_string("require 'digest/md5'");

  //  VALUE l=rb_gv_get("Digest::MD5");
  //  VALUE r=rb_funcall(l,rb_intern("digest"),1,rb_str_new2(p.c_str()));

  std::ostringstream os;
  os<<"s=''; Digest::MD5::digest('"<<p<<"').each_byte{|b|s+=sprintf('%X',b)};s";

  VALUE r=rb_eval_string(os.str().c_str());

  std::string s;

  s=STR2CSTR(r);

  return s;
}





bool rubyMatchRegex(const std::string &p,const std::string &pregex)
  {
    VALUE rp=rb_str_new(p.c_str(),p.length());
    VALUE rregex=rb_str_new(pregex.c_str(),pregex.length());
    
    VALUE ret=rb_reg_match(rp,rregex);
    return ret==Qtrue;
  }




unsigned char fromHexBS(unsigned char c) //throws(std::runtime_error)
  {
    if(c>='a' && c<='f')
      return c-'a'+0xA;
    if(c>='A' && c<='F')
      return c-'A'+0xA;

    if(c>='0' && c<='9')
      return c-'0';
    throw std::runtime_error("wrong input in fromHex()");
  }

void toHexBS(char c,std::ostream &os)
  {
    unsigned char c0=(c&0xF);
    unsigned char c1=((c>>4)&0xF);

    char a,b;
    //  std::ostringstream os;
    if(c0<10)
      a=(c0+'0');
    else
      a=((c0-10)+'A');

    if(c1<10)
      b=(c1+'0');
    else
      b=((c1-10)+'A');

    //  cdebug("a:"<<(int)a);
    //  cdebug("b:"<<(int)b);
    os<<b<<a;

    //  return os.str();
  }


std::string binaryToHex(const std::string &s,bool separators)
  {
    std::ostringstream os;

    for(size_t i=0;i<s.length();i++)
      {
        //      cdebug(i<<":"<<(int)s[i]);
        toHexBS(s[i],os);
        if(i>0 && separators)
          {
            if((i%32)==0)
              os<<std::endl;
            else if((i%4)==0)
              os<<" ";
          }
      }
    //  cdebug(os.getString());

    return os.str();//getString();
  }


std::string hexToBinary(const std::string &s)
  {
    std::ostringstream os;
    bool complete=false;
    char current=0;

    for(size_t i=0;i<s.length();i++)
      {
        char c=s[i];
        if((c>='a' && c<='f') || (c>='A' && c<='F') || (c>='0' && c<='9'))
          {
            if(complete)
              {
                current<<=4;
                current|=fromHexBS(c);
                os<<current;
                complete=false;
              }
            else
              {
                current=fromHexBS(c);
                complete=true;
              }
          }

      }


    return os.str();
  }
