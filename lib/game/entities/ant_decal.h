#ifndef __ANT_DECAL_H
#define __ANT_DECAL_H

#include "entity.h"

class AntDecal:public AntEntity {
  public:
    AntDecal ( AntMap* pMap );
    virtual ~AntDecal() throw();

    virtual void init();
    virtual void saveXML ( Node &node ) const;
    virtual void loadXML ( const Node &node );

    virtual AGString xmlName() const {
      return "antDecal";
    }

  private:
    std::string typeID;
};

#endif

