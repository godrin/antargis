#ifndef AG_STRING
#define AG_STRING

#include <rk_base.h>
#include <string>
#include <vector>
#include <SDL.h>


/** A string class that natively supports UTF-8 */
class AGEXPORT AGString:public std::string
{
public:
  static const size_t npos = static_cast<size_t>(-1);

  AGString();
  AGString(const std::string &p);
  AGString(const std::string &p,size_t many);
  AGString(const AGString &p);
  explicit AGString(int i);
  explicit AGString(size_t i);
  explicit AGString(long i);
  explicit AGString(float f);
  AGString(const char c,size_t many=1);
  AGString(const char*c);
  AGString(const char*c,size_t size);

#ifndef SWIiG
  AGString &operator=(const AGString &s);

  AGString &operator+=(const AGString &p);
#endif

  // size in bytes
  size_t memory() const;

  AGString substr(size_t from,size_t len) const;

  std::vector<AGString> split(const AGString &p) const;

  AGString replace(const AGString &what, const AGString &by) const;

  AGString last(size_t n) const;

  AGString operator+(const AGString &s) const;
  AGString operator+(const char *s) const;
  AGString operator+(const std::string &s) const;

  AGString operator*(size_t p) const;

  int toInt() const;
  long toLong() const;
  float toFloat() const;
  bool toBool() const;

  Uint8 toUint8() const;
  Sint16 toSint16() const;

  int parseHex() const;

  static AGString toHex(int i);

  bool matches(const std::string &regex) const;
};

#endif
