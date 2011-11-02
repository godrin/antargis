#ifndef AG_STRINGSTREAM
#define AG_STRINGSTREAM

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_string.h>

#include <SDL.h>


/// a text-based stream - you have to take care for separators yourself
/// possible seperators are:
///  ' '    ','    '_'  ';' '\n'
/// the standard behaviour on blanks and newlines is to eat them
class AGEXPORT AGStringStream
{
 public:
  AGStringStream(const AGString &buffer="");

  AGStringStream &operator<<(const int &i);
  AGStringStream &operator<<(const Uint8 &i);
  AGStringStream &operator<<(const Sint16 &i);
  AGStringStream &operator<<(const AGString &s);
  AGStringStream &operator<<(const float &f);

  const AGString &str() const;

  void precision(int i);

 private:
  AGString mBuffer;
  size_t readPos;
  int mPrecision;
  bool eatSeparators;
};




#endif
