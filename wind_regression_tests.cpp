#include "FourFeatures.hpp"
extern "C" int printf(const char*,...);
struct Fake { unsigned char data[0x3000]; JJAddr base; bool corruptReadback, failRestore; unsigned writeCount; };
static bool region(void*c,JJAddr at,JJRegion*out){Fake*f=(Fake*)c;if(at>=f->base+sizeof(f->data))return false;*out={f->base,sizeof(f->data),3};return true;}
static bool readmem(void*c,JJAddr at,void*out,JJSize n){Fake*f=(Fake*)c;if(at<f->base||at-f->base+n>sizeof(f->data))return false;memcpy(out,f->data+at-f->base,n);if(f->corruptReadback&&at==f->base+0x1900&&f->writeCount){JJWord fake=jj_float(0.6f);memcpy(out,&fake,4);}return true;}
static bool writemem(void*c,JJAddr at,const void*v,JJSize n){Fake*f=(Fake*)c;if(at<f->base||at-f->base+n>sizeof(f->data))return false;JJWord w=0;memcpy(&w,v,4);if(f->failRestore&&w==jj_float(.6f))return false;memcpy(f->data+at-f->base,v,n);f->writeCount++;return true;}
static void put(Fake&f,unsigned at,JJWord v){memcpy(f.data+at,&v,4);}
static JJWord get(Fake&f,unsigned at){JJWord v;memcpy(&v,f.data+at,4);return v;}
#define CHECK(x) if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}
int main(){Fake f={};f.base=0x7000000000ULL;JJIO io={&f,region,readmem,writemem,0,0};JJFeatureState s={};put(f,0x1800,jj_float(.83999997377f));put(f,0x1900,jj_float(.6f));
 CHECK(jj_apply_feature(io,JJ_WIND,true,s));CHECK(s.enabled&&s.verified);CHECK(get(f,0x1900)==jj_float(1314.0f));CHECK(jj_apply_feature(io,JJ_WIND,false,s));CHECK(!s.enabled&&!s.verified);
 f.writeCount=0;f.corruptReadback=true;CHECK(!jj_apply_feature(io,JJ_WIND,true,s));CHECK(!s.enabled&&!s.count);CHECK(get(f,0x1900)==jj_float(.6f));
 f.writeCount=0;f.failRestore=true;CHECK(!jj_apply_feature(io,JJ_WIND,true,s));CHECK(s.enabled&&!s.verified&&s.count==1);CHECK(get(f,0x1900)==jj_float(1314.0f));
 f.failRestore=false;f.corruptReadback=false;CHECK(jj_apply_feature(io,JJ_WIND,false,s));CHECK(!s.enabled&&!s.count);CHECK(jj_apply_feature(io,JJ_WIND,true,s));CHECK(s.enabled&&s.verified);
 printf("PASS: wind 1314, readback rollback, pending restore, recovery and retry\n");return 0;}
