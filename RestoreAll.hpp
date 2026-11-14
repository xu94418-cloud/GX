#pragma once
#include "FourFeatures.hpp"
struct JJRestoreSummary {JJSize attemptedFeatures,remainingFeatures,restored,failed,skipped;unsigned attemptedMask;};
static bool jj_restore_all(JJIO io,JJFeatureState *states,JJRestoreSummary &out){
 out={};for(unsigned i=0;i<5;i++){
  if(!states[i].enabled&&!states[i].count)continue;
  out.attemptedFeatures++;out.attemptedMask|=1U<<i;
  jj_apply_feature(io,i==4?JJ_SPEED:i,false,states[i]);
  out.restored+=states[i].report.restored;out.failed+=states[i].report.failed;out.skipped+=states[i].report.skipped;
  if(states[i].enabled||states[i].count)out.remainingFeatures++;
 }
 return out.remainingFeatures==0;
}
