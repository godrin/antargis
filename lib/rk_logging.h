/* 
 * File:   rk_logging.h
 * Author: davidkamphausen
 *
 * Created on 16. Februar 2009, 17:47
 */

#ifndef _RK_LOGGING_H
#define	_RK_LOGGING_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include "rk_base.h"
#include "rk_singleton.h"

#include <fstream>
#include <sstream>

class RKLogging : public RKSingleton<RKLogging> {
public:
  virtual ~RKLogging();

  void logDirect(const std::string &s);

  void logLine(const std::string &s, const std::string &pLevel);

  void log(const std::string &s, const std::string &pLevel);

  void finishLine();

  std::string getLog();
  void clear();
  void flush();
protected:
  RKLogging();

private:

  void startLine(const std::string &pLevel);

  std::ofstream output;
  friend class RKSingleton<RKLogging>;
  std::string mLastLevel;

};

namespace logger {

  class Special {
  public:

    enum Type {
      ENDL
    };
    Special(const Type &pType);

    Type getType() const;
  private:
    Type mType;
  };

  class Channel {
  public:

    enum Type {
      T_OUT, T_ERR, T_DEBUG, T_WARN, T_TRACE, T_INFO, T_GC
    };

    Channel(const Type &pType);
    ~Channel();

    Channel & operator<<(const std::string &s);

    Type getType() const;

    Channel & operator<<(const Special &pSpecial);

    template<class T>
    Channel & operator<<(const T&t) {
      std::ostringstream os;
      os << t;
      (*this) << os.str();
      return *this;
    };

    void flush();

    static std::string getType(const Type &pType);
  private:
    void check();

    Type mType;
    bool mEnabled;
    bool mChecked;
  };


  extern AGEXPORT Channel out;
  extern AGEXPORT Channel debug;
  extern AGEXPORT Channel warn;
  extern AGEXPORT Channel gc;
  extern AGEXPORT Channel err;
  extern AGEXPORT Channel trace;
  extern AGEXPORT Channel info;

  extern AGEXPORT Special endl;
}
#define Log(str) RKLogging::log()


#endif	/* _RK_LOGGING_H */

