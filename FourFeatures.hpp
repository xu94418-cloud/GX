#pragma once
#include "JJMemoryEngine.hpp"
enum JJFeature {JJ_SPEED=0,JJ_ARCHER=1,JJ_HEAVY=2,JJ_WIND=3};
struct JJFeatureReport {JJSize initial,joint1,joint2,refined,prepared,written,failed,skipped,restored;JJAddr bytesRead,readFailures;const char *error;};
struct JJWriteRecord {JJAddr address;JJWord original;};
struct JJFeatureState {bool enabled;bool verified;JJWriteRecord *records;JJSize count;JJWord activeValue;JJFeatureReport report;};
static JJWord jj_float(float f){JJWord w;memcpy(&w,&f,4);return w;}
static const char *jj_name(unsigned f){static const char *n[]={"四倍攻速","射手蓄力","重击大招","风主秒杀"};return n[f<4?f:0];}
static JJWord jj_applied(unsigned f){return f==JJ_ARCHER?888U:jj_float(f==JJ_SPEED?4.0f:f==JJ_HEAVY?6.0f:1314.0f);}
static JJWord jj_restored(unsigned f){return f==JJ_ARCHER?1082977991U:jj_float(f==JJ_SPEED?1.0f:f==JJ_HEAVY?5.0f:0.6f);}
static bool jj_apply_feature(JJIO io,unsigned f,bool enable,JJFeatureState &state,float speedValue=4.0f){
 JJFeatureReport rep={};JJWord applied=f==JJ_SPEED?jj_float(speedValue):jj_applied(f),restore=jj_restored(f);
 if(!enable){
  if(state.count)applied=state.activeValue;
  JJSize remain=0;
  for(JJSize i=0;i<state.count;i++){
   JJWriteRecord rec=state.records[i];JJWord now;
   if(!io.read(io.ctx,rec.address,&now,4)){rep.failed++;state.records[remain++]=rec;continue;}
   if(now==applied){if(io.write(io.ctx,rec.address,&restore,4))rep.restored++;else{rep.failed++;state.records[remain++]=rec;}}
   else if(now==restore)rep.restored++;else rep.skipped++;
  }
  state.count=remain;state.enabled=remain!=0;state.verified=false;if(!remain){jj_free(state.records);state.records=0;}if(remain)rep.error="部分地址还原失败，可再次点击重试";state.report=rep;return !remain;
 }
 if(state.enabled||state.count){rep.error=state.verified?"该功能已经开启":"存在待恢复记录，请先还原后重试";state.report=rep;return false;}
 JJMemoryEngine engine(io);JJWord needle;JJType type;JJRange range;
 if(f==JJ_SPEED){needle=jj_float(3.8013333e20f);type=JJ_F32;range={0x6000000000ULL,0x9000000000ULL};}
 else if(f==JJ_ARCHER){needle=1082977991;type=JJ_I32;range={0x6000000000ULL,0x9000000000ULL};}
 else if(f==JJ_HEAVY){needle=427281867;type=JJ_I32;range={0x6000000000ULL,0x9000000000ULL};}
 else{needle=jj_float(0.83999997377f);type=JJ_F32;range={0x6000000000ULL,0x9000000000ULL};}
 bool ok=engine.JJScanMemory(range,&needle,type);rep.initial=engine.getResultsCount();
 if(ok&&f==JJ_HEAVY){needle=6;ok=engine.JJNearBySearch(0x20,&needle,JJ_I32);rep.joint1=engine.getResultsCount();if(ok){needle=jj_float(5.0f);ok=engine.JJNearBySearch(0x20,&needle,JJ_F32);rep.joint2=engine.getResultsCount();}if(ok)ok=engine.JJScanMemory(range,&needle,JJ_F32);}
 if(ok&&f==JJ_WIND){needle=jj_float(0.6f);ok=engine.JJNearBySearch(0x500,&needle,JJ_F32);rep.joint1=engine.getResultsCount();if(ok)ok=engine.JJScanMemory(range,&needle,JJ_F32);}
 rep.refined=engine.getResultsCount();rep.bytesRead=engine.bytesRead;rep.readFailures=engine.readFailures;
 if(!ok){rep.error=engine.error?engine.error:"搜索失败";state.report=rep;return false;}
 if(!rep.refined){rep.error="没有符合条件的结果，请进入游戏场景后重试";state.report=rep;return false;}
 JJWriteRecord *records=(JJWriteRecord*)jj_alloc(rep.refined*sizeof(JJWriteRecord));if(!records){rep.error="记录申请失败";state.report=rep;return false;}
 const JJList &results=engine.getResults();
 for(JJSize i=0;i<results.count;i++){
  JJAddr a=results.data[i]+(f==JJ_SPEED?20:0);JJWord now;
  if(!engine.readMemory(a,&now,4)){rep.skipped++;continue;}
  if(f!=JJ_SPEED){JJWord expected=f==JJ_ARCHER?1082977991U:jj_float(f==JJ_HEAVY?5.0f:0.6f);if(now!=expected){rep.skipped++;continue;}}
  if(f==JJ_SPEED){float v;memcpy(&v,&now,4);if(!(v>=0.5f)){rep.skipped++;continue;}}
  records[rep.prepared++]={a,now};
 }
 if(!rep.prepared){jj_free(records);rep.error="候选没有通过最终判断或无法读取";state.report=rep;return false;}
 for(JJSize i=0;i<rep.prepared;i++){
  if(io.cancelled&&io.cancelled(io.ctx)){rep.error="已取消，正在回滚";break;}
  JJWriteRecord record=records[i];
  if(f!=JJ_SPEED){JJWord latest;if(!engine.readMemory(record.address,&latest,4)||latest!=record.original){rep.failed++;rep.error="目标数值已变化，正在回滚";break;}}
  if(!engine.JJWriteMemory(record.address,&applied,4)){rep.failed++;if(f==JJ_SPEED)continue;rep.error="写入失败，正在回滚";break;}
  // Preserve this record before verification: a successful write may have changed memory
  // even if the subsequent read fails, and rollback must still attempt restoration.
  records[rep.written++]=record;
  if(f==JJ_WIND){JJWord observed=0;
   if(!engine.readMemory(record.address,&observed,4)||observed!=applied){rep.failed++;rep.error="写入后校验失败，正在回滚";break;}
  }
 }
 if(f==JJ_SPEED&&!rep.error){
  if(!rep.written){jj_free(records);rep.error="符合条件的地址全部写入失败";state.report=rep;return false;}
  state.records=records;state.count=rep.written;state.enabled=true;state.verified=true;state.activeValue=applied;
  if(rep.failed)rep.error="部分地址写入失败，其余成功项已保留";
  state.report=rep;return rep.failed==0;
 }
 if(rep.error||rep.written!=rep.prepared){
  JJSize remain=0;for(JJSize i=0;i<rep.written;i++)if(!io.write(io.ctx,records[i].address,&records[i].original,4))records[remain++]=records[i];
  if(remain){state.records=records;state.count=remain;state.enabled=true;state.verified=false;state.activeValue=applied;rep.error="写入未完成且部分回滚失败，请点击功能重试还原";}else jj_free(records);
  state.report=rep;return false;
 }
 state.records=records;state.count=rep.prepared;state.enabled=true;state.verified=true;state.activeValue=applied;state.report=rep;return true;
}
