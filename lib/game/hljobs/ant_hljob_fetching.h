#ifndef __ANT_HLJOB_FETCHING_H
#define __ANT_HLJOB_FETCHING_H

#include "ant_hljob.h"
#include "ant_boss.h"

#include <set>
#include <map>

class AntHLJobFetching:public AntHLJob {
public:
    AntHLJobFetching(AntBoss* pBoss);

    virtual bool checkPerson(AntPerson*);
    virtual bool finished();
    virtual bool startTogether() const;
    virtual AGVector2 basePos();

    virtual AGString xmlName() const;
    virtual void saveXML(Node &node) const;
    virtual void loadXML(const Node &node);
protected:

    virtual std::vector<AntStockNeed> neededStock();

private:
    bool atHome(AntMan *man);
    bool checkFood(AntMan *man);
    void fetchForStock(std::vector<AntStockNeed> needed, AntMan* man);

    enum MODE {FETCH,REST};
    MODE mode;
    // for caching
    std::set<AntMan*> menAtHome;
};

#endif
