#include "FourFeatures.hpp"
extern "C" int printf(const char*,...);
struct Fake {unsigned char data[0x4000];JJAddr base;bool fail;unsigned writes;unsigned attempts,failAt;};
static bool reg(void *c,JJAddr cursor,JJRegion *out){Fake *f=(Fake*)c;if(cursor>=f->base+sizeof(f->data))return false;*out={f->base,sizeof(f->data),3};return true;}
static bool read(void *c,JJAddr a,void *out,JJSize n){Fake *f=(Fake*)c;if(a<f->base||n>sizeof(f->data)||a-f->base>sizeof(f->data)-n)return false;memcpy(out,f->data+a-f->base,n);return true;}
static bool write(void *c,JJAddr a,const void *v,JJSize n){Fake *f=(Fake*)c;f->attempts++;if(f->fail||(f->failAt&&f->attempts==f->failAt))return false;if(a<f->base||n>sizeof(f->data)||a-f->base>sizeof(f->data)-n)return false;memcpy(f->data+a-f->base,v,n);f->writes++;return true;}
static void put(Fake &f,unsigned o,JJWord v){memcpy(f.data+o,&v,4);}
static JJWord get(Fake &f,unsigned o){JJWord v;memcpy(&v,f.data+o,4);return v;}
#define CHECK(x) if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}
int main(){
 CHECK(jj_name(0)!=0);
 Fake f={};f.base=0x7000000000ULL;JJIO io={&f,reg,read,write,0,0};
 put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(1));put(f,0x120,jj_float(9));put(f,0x400,1082977991);put(f,0x800,427281867);put(f,0x810,6);put(f,0x818,jj_float(5));put(f,0x828,jj_float(5));put(f,0x1800,jj_float(0.83999997377f));put(f,0x1900,jj_float(0.6f));
 JJFeatureState states[4]={};
 for(unsigned i=0;i<4;i++){CHECK(jj_apply_feature(io,i,true,states[i]));CHECK(states[i].enabled);CHECK(states[i].report.written>0);}
 CHECK(get(f,0x114)==jj_float(4));CHECK(get(f,0x120)==jj_float(9));CHECK(get(f,0x400)==888);CHECK(get(f,0x818)==jj_float(6));CHECK(get(f,0x828)==jj_float(6));CHECK(get(f,0x1900)==jj_float(1314.0f));CHECK(get(f,0x800)==427281867);
 for(unsigned i=0;i<4;i++){CHECK(jj_apply_feature(io,i,false,states[i]));CHECK(!states[i].enabled);}
 CHECK(get(f,0x114)==jj_float(1));CHECK(get(f,0x400)==1082977991);CHECK(get(f,0x818)==jj_float(5));CHECK(get(f,0x1900)==jj_float(0.6f));
 // Real original machine-code probe: retain only anchors with neighbors, union results.
 memset(f.data,0,sizeof(f.data));put(f,0x100,427281867);put(f,0x300,427281867);unsigned nearby[]={0xdcU,0xe0U,0x110U,0x120U,0x124U};for(unsigned a:nearby)put(f,a,6);put(f,0x118,jj_float(5));put(f,0x128,jj_float(5));
 JJMemoryEngine e(io);JJWord needle=427281867;CHECK(e.JJScanMemory({f.base,f.base+sizeof(f.data)},&needle,JJ_I32));CHECK(e.getResultsCount()==2);needle=6;CHECK(e.JJNearBySearch(0x20,&needle,JJ_I32));JJAddr expected1[]={0xdc,0xe0,0x100,0x110,0x120};CHECK(e.getResultsCount()==5);for(unsigned i=0;i<5;i++)CHECK(e.getResults().data[i]==f.base+expected1[i]);needle=jj_float(5);CHECK(e.JJNearBySearch(0x20,&needle,JJ_F32));JJAddr expected2[]={0x100,0x110,0x118,0x120,0x128};CHECK(e.getResultsCount()==5);for(unsigned i=0;i<5;i++)CHECK(e.getResults().data[i]==f.base+expected2[i]);CHECK(e.JJScanMemory({f.base,f.base+sizeof(f.data)},&needle,JJ_F32));CHECK(e.getResultsCount()==2);
 // A low search ceiling must not reach the high-address fake region.
 JJMemoryEngine low(io);needle=427281867;CHECK(low.JJScanMemory({0,0x900000000ULL},&needle,JJ_I32));CHECK(low.getResultsCount()==0);
 // No writes if speed condition fails.
 memset(f.data,0,sizeof(f.data));put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(0.1f));put(f,0x120,jj_float(1));f.writes=0;CHECK(!jj_apply_feature(io,JJ_SPEED,true,states[0]));CHECK(f.writes==0);
 // Failed write must not mark a feature enabled.
 put(f,0x400,1082977991);f.fail=true;CHECK(!jj_apply_feature(io,JJ_ARCHER,true,states[1]));CHECK(!states[1].enabled);f.fail=false;
 // Restore skips an address whose content has changed independently.
 CHECK(jj_apply_feature(io,JJ_ARCHER,true,states[1]));put(f,0x400,123);CHECK(jj_apply_feature(io,JJ_ARCHER,false,states[1]));CHECK(get(f,0x400)==123);CHECK(states[1].report.skipped==1);
 // A failed second write must roll back the first one.
 memset(f.data,0,sizeof(f.data));put(f,0x800,427281867);put(f,0x810,6);put(f,0x818,jj_float(5));put(f,0x828,jj_float(5));f.attempts=0;f.failAt=2;
 CHECK(!jj_apply_feature(io,JJ_HEAVY,true,states[2]));CHECK(!states[2].enabled);CHECK(get(f,0x818)==jj_float(5));CHECK(get(f,0x828)==jj_float(5));f.failAt=0;
 // Speed follows the original per-candidate write behavior: one failure does not roll back another success.
 memset(f.data,0,sizeof(f.data));put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(1));put(f,0x200,jj_float(3.8013333e20f));put(f,0x214,jj_float(1));f.attempts=0;f.failAt=2;
 CHECK(!jj_apply_feature(io,JJ_SPEED,true,states[0]));CHECK(states[0].enabled);CHECK(states[0].count==1);CHECK(get(f,0x114)==jj_float(4));CHECK(get(f,0x214)==jj_float(1));f.failAt=0;CHECK(jj_apply_feature(io,JJ_SPEED,false,states[0]));
 // Custom speed reuses the same decimal +20 predicate/write path and restores its own actual applied value.
 float customValues[]={2.5f,0.25f,12.0f};
 for(float value:customValues){memset(f.data,0,sizeof(f.data));put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(1));JJFeatureState custom={};CHECK(jj_apply_feature(io,JJ_SPEED,true,custom,value));CHECK(get(f,0x114)==jj_float(value));CHECK(custom.activeValue==jj_float(value));CHECK(jj_apply_feature(io,JJ_SPEED,false,custom));CHECK(get(f,0x114)==jj_float(1));CHECK(!custom.enabled);}
 printf("PASS: custom speed apply/restore (2.5, 0.25, 12) and original four features\n");
 printf("PASS: four features enable/restore, high ranges, decimal +20 offset and +0x20 decoy, original nearby-search sets, failed writes, stale restore guard\n");return 0;
}
