#include <rk_debug.h>
#include <rk_string.h>
#include <rk_utf8.h>
#ifdef USE_RK_RTOOLS
#include <rk_rtools.h>
#endif
///////////
// AGString
///////////

AGString::AGString(int i) {
  std::ostringstream os;
  os << i;
  *this = os.str();
}
AGString::AGString(long i) {
  std::ostringstream os;
  os << i;
  *this = os.str();
}
AGString::AGString(size_t i) {
  std::ostringstream os;
  os << i;
  *this = os.str();
}
AGString::AGString(float f) {
  std::ostringstream os;
  os << f;
  *this = os.str();
}

AGString::AGString() {}

AGString::AGString(const char *c) : std::string(c) {}

AGString::AGString(const char *c, size_t size) : std::string(c, size) {}

AGString::AGString(const std::string &p) : std::string(p) {}
AGString::AGString(const std::string &p, size_t many) : std::string(p, many) {}
AGString::AGString(const AGString &p) : std::string(p) {}

AGString::AGString(const char c, size_t many) : std::string(c, many) {}

AGString &AGString::operator=(const AGString &p) {
  std::string::operator=(p);
  return *this;
}

AGString &AGString::operator+=(const AGString &p) {
  std::string::operator+=(p);
  return *this;
}

size_t AGString::memory() const { return length(); }

AGString AGString::substr(size_t from, size_t len) const {
  return AGString(std::string::substr(from, len));
}

std::vector<AGString> AGString::split(const AGString &p) const {
  std::vector<AGString> a;
  size_t oi = 0;
  size_t i = find(p);
  for (;;) {
    i = find(p, oi);
    a.push_back(substr(oi, i - oi));
    if (i == std::string::npos)
      break;
    oi = i + 1;
  }
  return a;
}

AGString AGString::operator+(const AGString &p) const {
  AGString n(*this);
  n += p;
  return n;
}
AGString AGString::operator+(const char *p) const {
  AGString n(*this);
  n += p;
  return n;
}
AGString AGString::operator+(const std::string &p) const {
  AGString n(*this);
  n += p;
  return n;
}

AGString AGString::replace(const AGString &what, const AGString &by) const {
  size_t i = 0;
  size_t oi = 0;
  AGString n;

  while (i != npos) {
    i = find(what, oi);
    n += substr(oi, i - oi);
    if (i == npos)
      break;
    n += by;
    oi = i + 1;
  }
  return n;
}

int AGString::toInt() const { return atoi(c_str()); }
long AGString::toLong() const { return atol(c_str()); }
float AGString::toFloat() const { return atof(c_str()); }

Uint8 AGString::toUint8() const { return toInt(); }
Sint16 AGString::toSint16() const { return toInt(); }

int AGString::parseHex() const {
  int i = 0;
  for (size_t j = 0; j < length(); j++) {
    int k = 0;
    i <<= 4;
    char c = operator[](j);
    if (c >= 'A' && c <= 'F')
      k = c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
      k = c - 'a' + 10;
    else if (c >= '0' && c <= '9')
      k = c - '0';
    i += k;
  }
  return i;
}

AGString AGString::toHex(int i) {
  std::string s;
  std::string c = "0";
  while (i != 0) {
    int k = i & 0xF;

    if (k > 9)
      c[0] = 'A' + k - 10;
    else
      c[0] = '0' + k;

    s = c + s;
    i >>= 4;
  }

  while (s.length() < 2)
    s = std::string("0") + s;
  return AGString(s);
}

bool AGString::toBool() const {
  if (length() == 0)
    return false;
  if (operator[](0) == '1')
    return true;
  if (substr(0, 4) == "true")
    return true;
  return false;
}

AGString AGString::operator*(size_t p) const {
  std::ostringstream os;
  for (size_t i = 0; i < p; i++)
    os << (*this);
  return AGString(os.str());
}

#ifdef USE_RK_RTOOLS
bool AGString::matches(const std::string &regex) const {
  return rubyMatchRegex(*this, regex);
}
#endif
AGString AGString::last(size_t n) const {
  throw std::runtime_error("not implemneted");
  return "";
}
