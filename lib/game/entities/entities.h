#ifndef __ENTITIES_H
#define __ENTITIES_H
#include <ag_xml.h>

class AntEntity;
class AntMap;
AntEntity *createEntity(const Node &node,AntMap *map);

#endif
