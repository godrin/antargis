#ifndef RK_CONFIG_H
#define RK_CONFIG_H

#include <string>
#include "rk_singleton.h"

#include <map>

class RKConfig:public RKSingleton<RKConfig>
{
  std::map<std::string, std::string> mData;

public:
  std::string get(const std::string &pKey) const;

protected:
  RKConfig();

private:

  friend class RKSingleton<RKConfig>;
};


#endif
