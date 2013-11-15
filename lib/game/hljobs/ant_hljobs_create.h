#ifndef __ANT_GAME_HLJOB_CREATE_H
#define __ANT_GAME_HLJOB_CREATE_H

class AntHLJob;
class AntBoss;
class Node;

namespace Antargis {
  AntHLJob *createFromXML(AntBoss *boss,const Node &n);
}
#endif

