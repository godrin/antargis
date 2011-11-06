#ifndef __HL_JOB_H
#define __HL_JOB_H

class AntBoss;

class AntHLJob {
public:
  AntHLJob(AntBoss *pBoss);
  
private:
  AntBoss *mBoss;
};

#endif
