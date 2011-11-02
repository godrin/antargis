
#include "rk_logging.h"
#include "rk_time.h"
#include "rk_config.h"
#include "rk_string.h"

#include <sstream>
#include <iostream>
#include <algorithm>

std::string operator*(const std::string &s, size_t i) {
  std::ostringstream os;

  for (size_t k = 0; k < i; k++)
    os << s;

  return os.str();
}


template
class RKSingleton<RKLogging>;

RKLogging::RKLogging() : output("rk_log.txt", std::ios_base::app) {
  mLastLevel = "init";

  std::string eq("=");
  logLine(eq * 60, "intrn");
  logLine("Starting Logging", "intrn");
  logLine(eq * 60, "intrn");
}

RKLogging::~RKLogging() {
  flush();
  logLine(std::string("=")*60, "intrn");
  logLine("Stopping Logging", "intrn");
  logLine(std::string("=")*60, "intrn");
  output.flush();
  output.close();
}

void RKLogging::log(const std::string &s, const std::string &pLevel) {

  // FIXME: do not start a new line each time !!
  if (allDestroyed())
    return;

  if (mLastLevel != pLevel)
    startLine(pLevel);

  size_t last = 0, i;
  std::string sub;
  for (i = s.find("\n"); i < s.length(); i = s.find("\n", i + 1)) {
    sub = s.substr(last, i - last);
    last = i;
    if (last > 0)
      logLine(sub, pLevel);
    else
      logDirect(sub);
  }
  sub = s.substr(last, i - last);
  if (last > 0)
    logLine(sub, pLevel);
  else
    logDirect(sub);
  mLastLevel = pLevel;
}

void RKLogging::startLine(const std::string &pLevel) {
  RKTime mtime;
  output << "[" << mtime.toString() << "] [" << pLevel << "] ";
}

void RKLogging::flush() {
  if (mLastLevel.length() > 0) {
    output << std::endl;
    output.flush();
    mLastLevel = "";
  }
}

void RKLogging::logLine(const std::string &s, const std::string &pLevel) {
  flush();
  startLine(pLevel);
  output << s;
  output << std::endl;
  output.flush();
  mLastLevel = "";
}

void RKLogging::logDirect(const std::string &s) {
  output << s;
  output.flush();
  mLastLevel = "internal";
}

void RKLogging::clear() {
  output.close();
  output.open("rk_log.txt");
}

std::string RKLogging::getLog() {
  output.flush();

  std::ifstream ifs("rk_log.txt");
  std::ostringstream os;
  std::string buf;

  while (!ifs.eof()) {
    ifs >> buf;
    os << buf;
  }
  return os.str();
}

namespace logger {

  Channel out(Channel::T_OUT);
  Channel debug(Channel::T_DEBUG);
  Channel warn(Channel::T_WARN);
  Channel err(Channel::T_ERR);
  Channel trace(Channel::T_TRACE);
  Channel info(Channel::T_INFO);
  Channel gc(Channel::T_GC);

  Special endl(Special::ENDL);

  Special::Special(const Type &pType) : mType(pType) {

  }

  Special::Type Special::getType() const {
    return mType;
  }

  Channel::Channel(const Type &pType) : mType(pType), mEnabled(true), mChecked(false) {
  }

  Channel::~Channel() {
    flush();

  }

  void Channel::flush() {
    RKLogging *instance = RKLogging::getInstance();
    if (instance)
      instance->flush();

  }

  Channel::Type Channel::getType() const {
    return mType;
  }

  Channel &Channel::operator<<(const Special &s) {
    check();
    if(mEnabled) {
      if (s.getType() == Special::ENDL)
        flush();
    }
    return *this;
  }

  void Channel::check() {
    if (!mChecked) {
      mChecked = true;
      AGString levels = RKConfig::getInstance()->get("logLevels");
      std::vector<AGString> ls = levels.split(",");
      if (std::find(ls.begin(), ls.end(), getType(getType())) == ls.end())
        mEnabled = false;
    }
  }

  Channel &Channel::operator<<(const std::string &s) {
    check();

    if(mEnabled) {
      RKLogging *clogger = RKLogging::getInstance();
      if (clogger)
        clogger->log(s, getType(mType));
    }
    return *this;
  }

  std::string Channel::getType(const Type &pType) {
    switch (pType) {
      case T_OUT:return "out  ";
      case T_ERR:return "error";
      case T_DEBUG:return "debug";
      case T_WARN:return "warn ";
      case T_TRACE:return "trace";
      case T_INFO:return "info ";
      case T_GC: return "gc   ";
      default: return "CRITICAL";
    }
  }
}

