#include "ag_serial.h"
#include <iostream>
#include <rk_debug.h>
#include "rk_rubyobj.h"

// INTEL is little endian

int mLittleEndian=-1;

bool littleEndian()
  {
    //return true;
    if(mLittleEndian<0)
      {
        Uint16 i=0x100;
        Uint8 *p=(Uint8*)&i;
        mLittleEndian=(p[0]!=0);
      }
    return mLittleEndian;
  }


/////////////////////////////////////////////////////////////
// BinaryIn
/////////////////////////////////////////////////////////////

BinaryIn::BinaryIn()
  {
  }
BinaryIn::~BinaryIn()
  {
  }

bool BinaryIn::eof()
  {
    return true;
  }
char BinaryIn::read()
  {
    throw SerialException();
    return 0;
  }

BinaryIn &BinaryIn::operator>>(Sint32 &i)
  {
    return operator>>((Uint32&)i);
  }
BinaryIn &BinaryIn::operator>>(Uint32 &u)
  {
    if(eof())
      throw SerialException();
    u=((Uint32(read())&0xFF))|((Uint32(read()&0xFF))<<8)|((Uint32(read())&0xFF)<<16)|((Uint32(read())&0xFF)<<24);

    return *this;
  }

BinaryIn &BinaryIn::operator>>(Uint16 &i)
  {
    if(eof())
      throw SerialException();
    i=((Uint16(read())&0xFF))|((Uint16(read())&0xFF)<<8);
    return *this;
  }

BinaryIn &BinaryIn::operator>>(float &f)
  {
    if(eof())
      throw SerialException();

    if(sizeof(float)!=4)
      {
        std::cerr<<"sizeof(float) should be 4 :-|"<<std::endl;
        throw SerialException();
      }
    if(!littleEndian())
      {
        // ok, load directly
        char *s=(char*)&f;
        s[0]=read();
        s[1]=read();
        s[2]=read();
        s[3]=read();
      }
    else
      {
        // ok, load swapped
        char *s=(char*)&f;
        s[3]=read();
        s[2]=read();
        s[1]=read();
        s[0]=read();
      }
    return *this;
  }

BinaryIn &BinaryIn::operator>>(std::string &f)
  {
    Uint32 l;
    (*this)>>l;

    cdebug("L:"<<l);

    assert(l<1000000);
    char *s=new char[l+1];
    char c;
    for(Uint32 i=0;i<l;i++)
      {
        c=read();
        cdebug("C:"<<(int)c);
        s[i]=c;
      }


    f=std::string(s,l);
    checkedDelete(s); // checked - no agrubyobject

    return *this;
  }


/////////////////////////////////////////////////////////////
// BinaryOut
/////////////////////////////////////////////////////////////

BinaryOut::BinaryOut()
  {
  }
BinaryOut::~BinaryOut()
  {
  }
void BinaryOut::write(char c)
  {
  }

BinaryOut &BinaryOut::operator<<(const Sint32 &i)
  {
    return operator<<((const Uint32&)i);
  }
BinaryOut &BinaryOut::operator<<(const Uint32 &u)
  {
    write((u>>0 )&0xFF);
    write((u>>8 )&0xFF);
    write((u>>16)&0xFF);
    write((u>>24)&0xFF);
    return *this;
  }

BinaryOut &BinaryOut::operator<<(const Uint16 &u)
  {
    write((u>>0 )&0xFF);
    write((u>>8 )&0xFF);
    return *this;
  }


BinaryOut &BinaryOut::operator<<(const float &f)
  {
    if(sizeof(float)!=4)
      {
        std::cerr<<"sizeof(float) should be 4 :-|"<<std::endl;
        throw SerialException();
      }
    if(!littleEndian())
      {
        const char *s=(const char*)&f;
        write(s[0]);
        write(s[1]);
        write(s[2]);
        write(s[3]);
      }
    else
      {
        const char *s=(const char*)&f;
        write(s[3]);
        write(s[2]);
        write(s[1]);
        write(s[0]);
      }

    return *this;
  }


BinaryOut &BinaryOut::operator<<(const std::string &s)
  {
    assert(s.size()<(1L<<31));

    (*this)<<Uint32(s.size());

    for(size_t i=0;i<s.length();i++)
      write(s[i]);
    return *this;
  }

/////////////////////////////////////////////////////////////
// BinaryFileIn
/////////////////////////////////////////////////////////////

BinaryFileIn::BinaryFileIn(const std::string &pName)
  {
    mEof=false;

    std::string n=findFile(pName);

    f=fopen(n.c_str(),"rb");
    p=0;
    is.unsetf(std::ios_base::skipws);
    size=0;
  }

BinaryFileIn::~BinaryFileIn()
  {
    fclose(f);
  }

size_t BinaryFileIn::pos() const
{
  return p;
}


bool BinaryFileIn::eof()
  {
    return mEof && size==0;
  }

char BinaryFileIn::read()
  {
    if(size==0)
      {
        if(mEof)
          {
            throw SerialException();
            return 0;
          }
        char buf[1001];
        size_t c=0;
        c=fread(buf,1,1000,f);
        is.str(std::string(buf,c));
        size+=c;
        if(c==0)
          mEof=true;
      }
    unsigned char c;
    is>>c;
    p++;
    size--;
    return c;
  }

void checkParentDirs(const std::string &s); // imported from ag_fs.cc

/////////////////////////////////////////////////////////////
// BinaryFileOut
/////////////////////////////////////////////////////////////
BinaryFileOut::BinaryFileOut(const std::string &pName)
  {
    std::string n=checkFileName(getWriteDir()+"/"+pName);
    checkParentDirs(n);

    f=fopen(n.c_str(),"wb");
    //  f=fopen(PHYSFS_openWrite(pName.c_str());
    assert(f);
    size=0;
    p=0;
  }
BinaryFileOut::~BinaryFileOut()
  {
    flush();
    fclose(f);
  }

size_t BinaryFileOut::pos() const
{
  return p;
}

void BinaryFileOut::flush()
  {
    std::string s=buffer.str();
    //  PHYSFS_write(f,s.c_str(),1,s.length());
    fwrite(s.c_str(),1,s.length(),f);
    buffer.str("");
    size=0;
  }

void BinaryFileOut::write(char c)
  {
    // non buffered for a start
    buffer<<c;
    size++;
    if(size>1024)
      flush();
    p++;
  }

/////////////////////////////////////////////////////////////
// BinaryStringIn
/////////////////////////////////////////////////////////////
BinaryStringIn::BinaryStringIn(const std::string &pText)
  {
    is.unsetf(std::ios_base::skipws);
    is.str(pText);
  }

BinaryStringIn::~BinaryStringIn()
  {
  }

bool BinaryStringIn::eof()
  {
    return is.eof();
  }
char BinaryStringIn::read()
  {
    char c;
    is>>c;
    return c;
  }


/////////////////////////////////////////////////////////////
// BinaryStringOut
/////////////////////////////////////////////////////////////
BinaryStringOut::BinaryStringOut()
  {
  }

BinaryStringOut::~BinaryStringOut()
  {
  }

void BinaryStringOut::write(char c)
  {
    os<<c;
  }


std::string BinaryStringOut::getString() const
{
  return os.str();
}
