#ifndef AG_STRING_UTF8
#define AG_STRING_UTF8

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_base.h>

#include <string>
#include <vector>
#include <SDL.h>

class AGStringUtf8;
class AGString;

class AGEXPORT AGCharUtf8
{
 public:
  explicit AGCharUtf8(const char p);
  explicit AGCharUtf8(const std::string &p);
  AGCharUtf8(const AGCharUtf8 &p);
  AGCharUtf8(AGStringUtf8 *ps,size_t ppos);

  bool operator==(const AGCharUtf8 &c) const;
  bool operator==(const char pChar) const;

#ifndef SWIG
  AGCharUtf8 &operator=(const AGCharUtf8 &c);
  AGCharUtf8 &operator=(const AGStringUtf8 &c);

  bool operator!=(const AGCharUtf8 &c) const;
  bool operator!=(const char pChar) const;
#endif


  std::string getString() const;

  size_t memory() const;

 private:
  std::string c;
  AGStringUtf8 *s;
  size_t pos;
};

class AGEXPORT AGStringUtf8Iterator
{

};

/** A string class that natively supports UTF-8 */
class AGEXPORT AGStringUtf8
{
 public:
#ifdef SWIG
   %rename(Npos) npos;
#endif
   static const size_t npos = static_cast<size_t>(-1);

  AGStringUtf8();
#ifndef SWIG
  explicit AGStringUtf8(const std::string &p);
  explicit AGStringUtf8(const std::string &p,size_t many);
#endif
  AGStringUtf8(const AGStringUtf8 &p);
#ifndef SWIG
  explicit AGStringUtf8(AGCharUtf8 c,size_t many=1);
#endif
  explicit AGStringUtf8(int i);
  explicit AGStringUtf8(size_t i);
  explicit AGStringUtf8(long i);
  explicit AGStringUtf8(float f);
#ifndef SWIG
  AGStringUtf8(const char c,size_t many=1);
#endif
  AGStringUtf8(const char*c);
  AGStringUtf8(const char*c,size_t size);


#ifdef SWIG
  %rename(set) operator=(const AGStringUtf8 &s);
  %rename(set) operator=(const AGCharUtf8 &s);
#endif
  AGStringUtf8 &operator=(const AGStringUtf8 &s);
  AGStringUtf8 &operator=(const AGCharUtf8 &s);

#ifndef SWIG
  AGStringUtf8 &operator+=(const AGStringUtf8 &p);
  AGStringUtf8 &operator+=(const AGCharUtf8 &p);
#endif
  // size in UTF-8 characters
  size_t length() const;

  // size in bytes
  size_t memory() const;

  AGStringUtf8 substr(size_t from,size_t len) const;

#ifndef SWIG
  AGCharUtf8 operator[](size_t i) const;
#endif

#ifdef SWIG
  %rename(__getitem__) operator[](size_t i);
#endif
  AGCharUtf8 operator[](size_t i);

#ifdef SWIG
  %rename(__setitem__) set(size_t i,const AGCharUtf8 &c);
#endif
  void set(size_t i,const AGCharUtf8 &c);


  std::vector<AGStringUtf8> split(const AGStringUtf8 &p) const;

  size_t find(const AGStringUtf8 &p,size_t pos=0) const;

  AGStringUtf8 replace(const AGStringUtf8 &what, const AGStringUtf8 &by) const;

  AGStringUtf8 operator+(const AGStringUtf8 &s) const;
  AGStringUtf8 operator+(const AGString &s) const;
  AGStringUtf8 operator+(const char *s) const;


  bool operator==(const AGStringUtf8 &p) const;

#ifndef SWIG
  bool operator!=(const AGStringUtf8 &p) const;
#endif

  bool operator<(const AGStringUtf8 &p) const;

#ifdef SWIG
  %rename(to_s) toString() const;
#endif
  std::string toString() const;
  std::string ascii() const;

  int toInt() const;
  long toLong() const;
  float toFloat() const;
  bool toBool() const;

  Uint8 toUint8() const;
  Sint16 toSint16() const;

  int parseHex() const;

  static AGStringUtf8 toHex(int i);

 private:

  std::string get(size_t pos) const;
  void set(size_t pos,const std::string &s);

  std::string s;
  mutable size_t mLength;

  friend class AGCharUtf8;

};

AGEXPORT std::ostream &operator<<(std::ostream &o,const AGStringUtf8 &s);


#endif
