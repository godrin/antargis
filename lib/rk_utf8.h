#ifndef AG_UTF8_H
#define AG_UTF8_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <rk_base.h>
#include <string>
#include <vector>
#include <SDL.h>

AGEXPORT size_t agFirstCharSize(const std::string &s,size_t pos=0);
AGEXPORT size_t agStringLength(const std::string &s);
AGEXPORT std::string agSubStr(const std::string &s,size_t from,size_t len);
AGEXPORT std::string unicode2Utf8(Uint16 unicode);

#endif
