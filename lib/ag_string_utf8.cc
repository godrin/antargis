#include <ag_string_utf8.h>
#include <rk_string.h>
#include <rk_debug.h>
#include <rk_utf8.h>

///////////////////////////////////////////////////////////////////////////
// Classes for UTF-8 support
///////////////////////////////////////////////////////////////////////////


///////////
// AGCharUtf8
///////////

AGCharUtf8::AGCharUtf8(char p)
  {
    c=std::string(p,1);
    s=0;
    pos=0;
  }

AGCharUtf8::AGCharUtf8(const std::string &p)
  {
    c=p;
    s=0;
    pos=0;
  }

AGCharUtf8::AGCharUtf8(const AGCharUtf8 &p):
  c(p.c),
  s(p.s),
  pos(p.pos)
  {

  }
AGCharUtf8::AGCharUtf8(AGStringUtf8 *ps,size_t ppos)
  {
    c=ps->get(ppos);
    s=ps;
    pos=ppos;
  }

AGCharUtf8 &AGCharUtf8::operator=(const AGStringUtf8 &p)
  {
    if(!s)
      {
        cdebug("Possible error while assigning constant AGCharUtf8");
        if(c.length()!=1)
          cdebug("Probable error while assigning AGCharUtf8 - string-length!=1 !");
        else
          c=p[0].c;
      }
    else
      {
        if(c.length()!=1)
          cdebug("Probable error while assigning AGCharUtf8 - string-length!=1 !");
        else
          {
            s->set(pos,c=p[0].c);
          }
      }
    return *this;
  }


AGCharUtf8 &AGCharUtf8::operator=(const AGCharUtf8 &p)
  {
    if(!s)
      cdebug("Possible error while assigning constant AGCharUtf8");
    else
      {
        s->set(pos,p.c);
      }
    c=p.c;
    return *this;
  }

bool AGCharUtf8::operator==(const AGCharUtf8 &p) const
{
  return c==p.c;
}

bool AGCharUtf8::operator==(const char pChar) const
{
  if(c.length()==1)
    return c[0]==pChar;
  return false;
}

bool AGCharUtf8::operator!=(const AGCharUtf8 &p) const
{
  return c!=p.c;
}

bool AGCharUtf8::operator!=(const char pChar) const
{
  if(c.length()==1)
    return c[0]!=pChar;
  return true;
}



std::string AGCharUtf8::getString() const
{
  return c;
}


size_t AGCharUtf8::memory() const
{
  return agFirstCharSize(c);
}


///////////
// AGStringUtf8
///////////

AGStringUtf8::AGStringUtf8(int i)
  {
    std::ostringstream os;
    os<<i;
    s=os.str();
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(long i)
  {
    std::ostringstream os;
    os<<i;
    s=os.str();
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(size_t i)
  {
    std::ostringstream os;
    os<<i;
    s=os.str();
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(float f)
  {
    std::ostringstream os;
    os<<f;
    s=os.str();
    mLength=npos;
  }


AGStringUtf8::AGStringUtf8()
  {
    mLength=npos;
  }

AGStringUtf8::AGStringUtf8(const char*c)
  {
    //CTRACE;
    s=c;
    mLength=npos;
  }

AGStringUtf8::AGStringUtf8(const char*c,size_t size)
  {
    //CTRACE;
    s=std::string(c,size);
    mLength=npos;
  }

AGStringUtf8::AGStringUtf8(const std::string &p)
  {
    //CTRACE;
    s=p;
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(const std::string &p,size_t many)
  {
    //CTRACE;
    for(size_t i=0;i<many;i++)
      s+=p;
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(const AGStringUtf8 &p)
  {
    //CTRACE;
    s=p.s;
    mLength=npos;
  }
AGStringUtf8::AGStringUtf8(AGCharUtf8 c,size_t many)
  {
    //CTRACE;
    for(size_t i=0;i<many;i++)
      operator+=(c);
    mLength=npos;
  }

AGStringUtf8::AGStringUtf8(const char c,size_t many)
  {
    //CTRACE;
    s=std::string(c,many);
    mLength=npos;
  }

AGStringUtf8 &AGStringUtf8::operator=(const AGStringUtf8 &p)
  {
    //CTRACE;
    s=p.s;
    mLength=npos;
    return *this;
  }
AGStringUtf8 &AGStringUtf8::operator=(const AGCharUtf8 &pChar)
  {
    //CTRACE;
    s=pChar.getString();
    mLength=npos;
    return *this;
  }


AGStringUtf8 &AGStringUtf8::operator+=(const AGStringUtf8 &p)
{
  //CTRACE;
  s+=p.s;
  mLength=npos;
  return *this;
}
AGStringUtf8 &AGStringUtf8::operator+=(const AGCharUtf8 &p)
{
  //CTRACE;
  s+=p.getString();
  mLength=npos;
  return *this;
}


size_t AGStringUtf8::length() const
{
  if(mLength==npos)
    mLength=agStringLength(s);
  return mLength;
}

size_t AGStringUtf8::memory() const
{
  return s.length();
}

AGStringUtf8 AGStringUtf8::substr(size_t from,size_t len) const
{
  AGStringUtf8 ns(agSubStr(s,from,len));
  //  ns.mLength=len;
  return ns;
}

AGCharUtf8 AGStringUtf8::operator[](size_t i) const
{
  return AGCharUtf8(agSubStr(s,i,1));
}
AGCharUtf8 AGStringUtf8::operator[](size_t i)
{
  return AGCharUtf8(this,i);
}

size_t AGStringUtf8::find(const AGStringUtf8 &p,size_t pos) const
{
  size_t c=0;
  size_t i=0;
  while(i<pos && c<s.length())
    {
      c+=agFirstCharSize(s,c);
      i++;
    }
  if(i<pos)
    return std::string::npos; // not even pos not found

  while(c<s.length())
    {
      if(s.substr(c,p.s.length())==p.s)
        return i;
      c+=agFirstCharSize(s,c);
      i++;
    }
  return npos;
}


std::vector<AGStringUtf8> AGStringUtf8::split(const AGStringUtf8 &p) const
{
  std::vector<AGStringUtf8> a;
  size_t oi=0;
  size_t i=find(p);
  for(;;)
    {
      i=find(p,oi);
      a.push_back(substr(oi,i-oi));
      if(i==std::string::npos)
        break;
      oi=i+1;
    }
  return a;
}

AGStringUtf8 AGStringUtf8::operator+(const AGStringUtf8 &p) const
{
  AGStringUtf8 n(*this);
  n+=p;
  return n;
}

AGStringUtf8 AGStringUtf8::operator+(const AGString &s) const
{
  AGStringUtf8 n(*this);
  n+=AGStringUtf8(s);
  return n;
}

AGStringUtf8 AGStringUtf8::operator+(const char *s) const
{
  AGStringUtf8 n(*this);
  n+=AGStringUtf8(s);
  return n;
}


bool AGStringUtf8::operator==(const AGStringUtf8 &p) const
{
  return s==p.s;
}

bool AGStringUtf8::operator!=(const AGStringUtf8 &p) const
{
  return s!=p.s;
}

bool AGStringUtf8::operator<(const AGStringUtf8 &p) const
{
  return s<p.s;
}

std::string AGStringUtf8::toString() const
{
  return s;
}

std::string AGStringUtf8::ascii() const
{
  size_t p=npos;
  for(size_t i=0;i<s.length();i++)
    {
      char c=s[i];
      if((c&0x80)!=0)
        {
          p=i;
        }
    }
  if(p!=npos)
    {
      cdebug("WARNING: Probable problem within ascii for position "<<p<<" in string:"<<s);
    }
  return s;
}


AGStringUtf8 AGStringUtf8::replace(const AGStringUtf8 &what, const AGStringUtf8 &by) const
{
  size_t i=0;
  size_t oi=0;
  AGStringUtf8 n;

  while(i!=npos)
    {
      i=find(what,oi);
      n+=substr(oi,i-oi);
      if(i==npos)
        break;
      n+=by;
      oi=i+1;
    }
  return n;
}


int AGStringUtf8::toInt() const
{
  return atoi(s.c_str());
}
long AGStringUtf8::toLong() const
{
  return atol(s.c_str());
}
float AGStringUtf8::toFloat() const
{
  return atof(s.c_str());
}

Uint8 AGStringUtf8::toUint8() const
{
  long i;
  std::istringstream is;
  is.str(s);
  is>>i;
  return i;
}
Sint16 AGStringUtf8::toSint16() const
{
  long i;
  std::istringstream is;
  is.str(s);
  is>>i;
  return i;
}


int AGStringUtf8::parseHex() const
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

AGStringUtf8 AGStringUtf8::toHex(int i)
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
    return AGStringUtf8(s);
  }

bool AGStringUtf8::toBool() const
{
  if(s.length()==0)
    return false;
  if(s[0]=='1')
    return true;
  if(s.substr(0,4)=="true")
    return true;
  return false;
}


std::string AGStringUtf8::get(size_t pos) const
{
  return agSubStr(s,pos,1);
}
void AGStringUtf8::set(size_t pos,const std::string &pStr)
  {
    mLength=npos;
    s=agSubStr(s,0,pos)+pStr+agSubStr(s,pos+1,std::string::npos);
  }

std::ostream &operator<<(std::ostream &o,const AGStringUtf8 &s)
  {
    o<<s.toString();
    return o;
  }

void AGStringUtf8::set(size_t i,const AGCharUtf8 &c)
  {
    operator[](i)=c;
  }
