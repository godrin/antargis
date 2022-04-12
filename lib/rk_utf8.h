#ifndef AG_UTF8_H
#define AG_UTF8_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <SDL.h>
#include <rk_base.h>
#include <string>
#include <vector>

/// get size of first utf8-character in string *s* beginning with byte position
/// *pos*
AGEXPORT size_t agFirstCharSize(const std::string &s, size_t pos = 0);
/// string-length in characters of utf8 string *s*
AGEXPORT size_t agStringLength(const std::string &s);
/// utf8-compatible substring of s
AGEXPORT std::string agSubStr(const std::string &s, size_t from, size_t len);
// convert 2-byte character to utf8
AGEXPORT std::string unicode2Utf8(Uint16 unicode);

#endif
