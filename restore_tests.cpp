#include "RestoreAll.hpp"
extern "C" int printf(const char*,...);
struct FakeRestore {unsigned char data[0x4000];JJAddr base,failAddress;unsigned writes;};
static bool region(void*c,JJAddr cursor,JJRegion*out){FakeRestore*f=(FakeRestore*)c;if(cursor>=f->base+sizeof(f->data))return false;*out={f->base,sizeof(f->data),3};return true;}
static bool read(void*c,JJAddr a,void*out,JJSize n){FakeRestore*f=(FakeRestore*)c;if(a<f->base||n>sizeof(f->data)||a-f->base>sizeof(f->data)-n)return false;memcpy(out,f->data+a-f->base,n);return true;}
static bool write(void*c,JJAddr a,const void*v,JJSize n){FakeRestore*f=(FakeRestore*)c;if(a==f->failAddress||a<f->base||n>sizeof(f->data)||a-f->base>sizeof(f->data)-n)return false;memcpy(f->data+a-f->base,v,n);f->writes++;return true;}
static void put(FakeRestore&f,unsigned a,JJWord v){memcpy(f.data+a,&v,4);}
static JJWord get(FakeRestore&f,unsigned a){JJWord v;memcpy(&v,f.data+a,4);return v;}
static void init(FakeRestore&f){memset(&f,0,sizeof(f));f.base=0x7000000000ULL;put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(1));put(f,0x400,1082977991);put(f,0x800,427281867);put(f,0x810,6);put(f,0x818,jj_float(5));put(f,0x828,jj_float(5));put(f,0x1800,jj_float(0.83999997377f));put(f,0x1900,jj_float(.6f));}
#define CHECK(x) if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}
int main(){CHECK(jj_name(0)!=0);FakeRestore f;init(f);JJIO io={&f,region,read,write,0,0};JJFeatureState states[5]={};JJRestoreSummary out;
 CHECK(jj_restore_all(io,states,out));CHECK(out.attemptedFeatures==0&&f.writes==0);
 for(unsigned i=0;i<4;i++)CHECK(jj_apply_feature(io,i,true,states[i]));
 CHECK(jj_restore_all(io,states,out));CHECK(out.attemptedFeatures==4&&out.remainingFeatures==0&&out.restored==5&&out.failed==0);for(unsigned i=0;i<5;i++)CHECK(!states[i].enabled);
 CHECK(get(f,0x114)==jj_float(1)&&get(f,0x400)==1082977991&&get(f,0x818)==jj_float(5)&&get(f,0x1900)==jj_float(.6f));
 CHECK(jj_apply_feature(io,JJ_SPEED,true,states[4],2.5f));CHECK(jj_restore_all(io,states,out));CHECK(out.attemptedFeatures==1&&out.attemptedMask==16&&get(f,0x114)==jj_float(1));
 for(unsigned i=0;i<4;i++)CHECK(jj_apply_feature(io,i,true,states[i]));f.failAddress=f.base+0x400;
 CHECK(!jj_restore_all(io,states,out));CHECK(out.remainingFeatures==1&&out.failed==1&&out.attemptedFeatures==4);CHECK(states[1].enabled&&!states[0].enabled&&!states[2].enabled&&!states[3].enabled);f.failAddress=0;CHECK(jj_restore_all(io,states,out));CHECK(out.restored==1&&out.remainingFeatures==0);
 CHECK(jj_apply_feature(io,JJ_ARCHER,true,states[1]));put(f,0x400,123);CHECK(jj_restore_all(io,states,out));CHECK(out.skipped==1&&out.remainingFeatures==0&&get(f,0x400)==123);
 printf("PASS: restore-all empty state, all four, custom multiplier, per-feature failure continuation/retry, changed-address skip\n");return 0;}
