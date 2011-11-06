#ifndef RESOURCE_H
#define RESOURCE_H

// INCLUDE_SWIG - used to filter, which files are included in swig-interfacing

#include <map>
#include <ag_xml.h>



/**
   Resource manages resources with BoA. Every entity has some kind of "bag", where
   everything is stored. This is it. Resources are identified by strings.
   You can get,set,sub and add resources.

   
FIXME: maybe remove Resource from c++??

*/
class AGEXPORT Resource
{
  std::map<std::string,float> r;
 public:
  Resource();

  void saveXML(Node &node) const;
  void loadXML(const Node &node);

  float get(const std::string &pName);
  void add(const std::string &pName,float value);
  void sub(const std::string &pName,float value);
  void set(const std::string &pName,float value);

  std::map<std::string,float> getAll() const;

  /// take everything, that's in r and put it into "this"
  void takeAll(Resource &r);

  /// take everything of type pname, that's in r and put it into "this"
  void take(Resource &r,const std::string &pName);

  /// check if the "bag" is empty
  bool empty() const;
};


#endif
