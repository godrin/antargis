#ifndef AG_RTOOLS_H
#define AG_RTOOLS_H

#include <rk_base.h>
#include <string>

AGEXPORT std::string rubyHash(const std::string &p);
AGEXPORT bool rubyMatchRegex(const std::string &p,const std::string &pregex);
std::string rubySub(const std::string &p,const std::string &search,const std::string &repl);

AGEXPORT std::string binaryToHex(const std::string &s,bool separators=true);
AGEXPORT std::string hexToBinary(const std::string &s);

#endif
