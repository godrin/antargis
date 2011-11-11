#ifndef __HL_JOB_H
#define __HL_JOB_H

class AntBoss;
class AntPerson;

class AntHLJob {
public:
  AntHLJob(AntBoss *pBoss);
  
  void assignJob(AntPerson *p);
  
private:
  AntBoss *mBoss;
};

#endif
