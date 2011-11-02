#include "ag_stringstream.h"

#include <sstream>

AGStringStream::AGStringStream(const AGString &buffer):
  mBuffer(buffer),readPos(0),eatSeparators(true)
{
  mPrecision=10;
}

AGStringStream &AGStringStream::operator<<(const Uint8 &i)
{
  mBuffer+=AGString(i);
  return *this;
}
AGStringStream &AGStringStream::operator<<(const Sint16 &i)
{
  mBuffer+=AGString(i);
  return *this;
}
AGStringStream &AGStringStream::operator<<(const int &i)
{
  mBuffer+=AGString(i);
  return *this;
}
AGStringStream &AGStringStream::operator<<(const AGString &s)
{
  mBuffer+=s;
  return *this;
}

void AGStringStream::precision(int i)
{
  mPrecision=i;
}

AGStringStream &AGStringStream::operator<<(const float &f)
{
  std::ostringstream os;
  os.precision(mPrecision);
  os<<f;
  mBuffer+=AGString(os.str());
  return *this;
}



const AGString &AGStringStream::str() const
{
  return mBuffer;
}

