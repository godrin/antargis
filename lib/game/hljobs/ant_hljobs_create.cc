#include "ant_hljobs_create.h"

#include "ant_hljob_dismiss.h"
#include "ant_hljob_fetching.h"      
#include "ant_hljob_fighting.h"      
#include "ant_hljob.h"
#include "ant_hljob_recruit.h"
#include "ant_hljob_drop.h"
#include "ant_hljob_fight_animal.h"
#include "ant_hljob_gather_and_do.h"  
#include "ant_hljob_moving.h"
#include "ant_hljob_rest.h"
#include "ant_hljob_pickup_from.h"


namespace Antargis {
  AntHLJob *createFromXML(AntBoss *boss,const Node &n) {
    AntHLJob *job=0;
    AGString jobName=n.getName();
    if(jobName=="hljob") // old data
      jobName=n.get("type");
    if(jobName=="hljobDismiss") {
      job=new AntHLJobDismiss(boss);
    } else if(jobName=="hljobRest" || jobName=="AntHeroRestJob") {
      job=new AntHLJobRest(boss);
    } else if(jobName=="hljobFetching") {
      job=new AntHLJobFetching(boss);
    } else if(jobName=="hljobPickupFrom") {
      job=new AntHLJobMoving(boss);
    } else if(jobName=="hljobMoving") {
      job=new AntHLJobMoving(boss);
    } else {
      throw std::runtime_error("Job type unknown yet! Name:"+jobName);
    }
    if(job)
      job->loadXML(n);
    return job;
  }
}
