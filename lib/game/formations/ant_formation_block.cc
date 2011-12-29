#include "ant_formation_block.h"
#include <cmath>
#include <algorithm>
#include "ant_boss.h"
#include "ant_person.h"

AntFormationBlock::AntFormationBlock(AntBoss* pboss, const AGVector2 & pdir): AntFormation(pboss),dir(pdir)
{
    CTRACE;
}


std::map< AntPerson*, AGVector2 > AntFormationBlock::computeFormation()
{
    CTRACE;
    // virtual positions as map from man to pair of [row,line (circle)]
    std::map<AntPerson*,std::pair<size_t,size_t> >  vpos;
    std::map<AntPerson*,AGVector2> rpos;

    std::vector<AntPerson*> men=getSortedMen();

    std::map<size_t,size_t> linesizes;
    size_t line=0;
    size_t row=0;
    size_t rowsize=5;

    AGVector2 normal=dir.normal();

    for (std::vector<AntPerson*>::iterator menIterator=men.begin();menIterator!=men.end();menIterator++)  {
        vpos[*menIterator]=std::make_pair(row,line);
        linesizes[line]++;
        row+=1;
        if (row>=rowsize) { // add check for new weapon group here
            row-=rowsize;
            line+=1;
        }
    }
    for (std::vector<AntPerson*>::iterator menIterator=men.begin();menIterator!=men.end();menIterator++)  {
        std::map<AntPerson*,std::pair<size_t,size_t> >::iterator  curvpos=vpos.find(*menIterator);
        size_t row=curvpos->second.first,line=curvpos->second.second;
        //float radius=line*1.2;
        //float angle=((float)row)/linesizes[line]*M_PI*2.0;

        size_t linesize=linesizes[line];
        rpos[*menIterator]=normal*(row-(linesize/2.0))*0.5 + dir*line*0.7;


        //rpos[*menIterator]=AGVector2(cos(angle)*radius,sin(angle)*radius);
    }
    rpos[dynamic_cast<AntPerson*>(getBoss()->getEntity())]=AGVector2(0,0);
    return rpos;

}


void AntFormationBlock::sortMen(std::vector< AntPerson* >& v)
{
    std::sort(v.begin(),v.end(),AntManSortBetterWeapon());

    AntPerson *bossPerson=dynamic_cast<AntPerson*>(getBoss()->getEntity());

    // put boss at 3rd position
    std::vector<AntPerson*> n;
    size_t i;
    // fill n up to 2 persons
    for (i=0;n.size()<2 && i<v.size();i++) {
        // don't copy boss
        if (!dynamic_cast<AntBoss*>(v[i]))
            n.push_back(v[i]);
    }
    n.push_back(bossPerson);
    for (;i<v.size();i++) {
        if (!dynamic_cast<AntBoss*>(v[i]))
            n.push_back(v[i]);
    }
    v=n;

}
