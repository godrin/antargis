#include "ant_formation_rest.h"
#include "ant_boss.h"
#include "ant_person.h"

#include <cmath>
#include <algorithm>

AntFormationRest::AntFormationRest(AntBoss* pboss): AntFormation(pboss)
{
    inited=false;
}


size_t getRowsOfLine(size_t line) {
    switch (line) {
    case 1:
        return 10;
    case 2:
        return 14;
    case 3:
        return 20;
    case 4:
        return 40;
    default:
        return 100;
    }
}

void AntFormationRest::computeFormation()
{
    // virtual positions as map from man to pair of [row,line (circle)]
    std::map<AntPerson*,std::pair<size_t,size_t> >  vpos;

    std::vector<AntPerson*> men=getSortedMen();

    std::map<size_t,size_t> linesizes;
    size_t line=1;
    size_t row=0;
    for (std::vector<AntPerson*>::iterator menIterator=men.begin();menIterator!=men.end();menIterator++)  {
        vpos[*menIterator]=std::make_pair(row,line);
        linesizes[line]++;
        row+=1;
        if (row>getRowsOfLine(line)) { // add check for new weapon group here
            row-=getRowsOfLine(line);
            line+=1;
        }
    }
    for (std::vector<AntPerson*>::iterator menIterator=men.begin();menIterator!=men.end();menIterator++)  {
        std::map<AntPerson*,std::pair<size_t,size_t> >::iterator  curvpos=vpos.find(*menIterator);
        size_t row=curvpos->second.first,line=curvpos->second.second;
        float radius=line*1.2;
        float angle=((float)row)/linesizes[line]*M_PI*2.0;
        realPositions[*menIterator]=AGVector2(cos(angle)*radius,sin(angle)*radius);
    }
    realPositions[dynamic_cast<AntPerson*>(getBoss()->getEntity())]=AGVector2(0,0);
    if (men.size()>0)
        inited=true;


}


AGVector2 AntFormationRest::getPositionReal(AntPerson* p)
{
    if (realPositions.find(p)==realPositions.end())
        computeFormation();
    return realPositions[p];
}

void AntFormationRest::sortMen(std::vector< AntPerson* >& v)
{
    std::sort(v.begin(),v.end(),AntManSortBetterWeapon());
}

