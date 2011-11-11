#ifndef __ANT_SACK_H
#define __ANT_SACK_H

#include "entity.h"

class AntSack:public AntEntity {
public:
    AntSack(AntMap* pMap);
    void init();

    void eventDie();
    void resourceChanged();
private:
    void checkResource(const AGString &s);
};

#endif
