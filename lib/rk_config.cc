#include "rk_config.h"

std::string RKConfig::get(const std::string &pKey) const {
  std::map<std::string, std::string>::const_iterator i=mData.find(pKey);
  if(i==mData.end())
    return "";
  return i->second;
}

RKConfig::RKConfig() {
  mData["logLevels"]="out,debug,warn,err,info";
}
