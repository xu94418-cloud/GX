#pragma once
// Independent compatibility implementation reconstructed from observed JJMemoryEngine behavior.
typedef unsigned long long JJAddr;
typedef unsigned long JJSize;
typedef unsigned int JJWord;
extern "C" void *malloc(JJSize);
extern "C" void free(void *);
extern "C" void *memcpy(void *,const void *,JJSize);
extern "C" void *memset(void *,int,JJSize);
extern "C" void qsort(void *,JJSize,JJSize,int(*)(const void*,const void*));
struct JJBlock {JJBlock *next,*prev;JJSize bytes;};
static JJBlock *jj_blocks;
static void *jj_alloc(JJSize n){if(n>0x40000000UL)return 0;JJBlock *b=(JJBlock*)malloc(n+sizeof(JJBlock));if(!b)return 0;b->bytes=n;b->prev=0;b->next=jj_blocks;if(jj_blocks)jj_blocks->prev=b;jj_blocks=b;return b+1;}
static void jj_free(void *p){if(!p)return;JJBlock *b=(JJBlock*)p-1;if(b->prev)b->prev->next=b->next;else jj_blocks=b->next;if(b->next)b->next->prev=b->prev;free(b);}
struct JJList {
 JJAddr *data;JJSize count,capacity;bool ok;
 JJList():data(0),count(0),capacity(0),ok(true){}
 ~JJList(){jj_free(data);}
 bool append(JJAddr a){if(count==capacity){JJSize n=capacity?capacity*2:256;if(n>1048576){ok=false;return false;}JJAddr *p=(JJAddr*)jj_alloc(n*sizeof(JJAddr));if(!p){ok=false;return false;}if(count)memcpy(p,data,count*sizeof(JJAddr));jj_free(data);data=p;capacity=n;}data[count++]=a;return true;}
 void swap(JJList &b){JJAddr *p=data;data=b.data;b.data=p;JJSize n=count;count=b.count;b.count=n;n=capacity;capacity=b.capacity;b.capacity=n;bool k=ok;ok=b.ok;b.ok=k;}
 static int cmp(const void *a,const void *b){JJAddr x=*(const JJAddr*)a,y=*(const JJAddr*)b;return x<y?-1:x>y?1:0;}
 void unique(){if(count<2)return;qsort(data,count,sizeof(JJAddr),cmp);JJSize n=1;for(JJSize i=1;i<count;i++)if(data[i]!=data[n-1])data[n++]=data[i];count=n;}
};
struct JJRange {JJAddr start,end;};
struct JJRegion {JJAddr start,size;JJWord protection;};
enum JJType {JJ_F32=4,JJ_I32=6};
struct JJIO {
 void *ctx;
 bool (*region)(void*,JJAddr,JJRegion*);
 bool (*read)(void*,JJAddr,void*,JJSize);
 bool (*write)(void*,JJAddr,const void*,JJSize);
 bool (*excluded)(void*,JJAddr);
 bool (*cancelled)(void*);
};
class JJMemoryEngine {
 JJIO io;bool scanned;JJList values;
 bool stop(){return io.cancelled&&io.cancelled(io.ctx);}
 bool equal(JJWord a,JJWord b,JJType t){if(t==JJ_F32){float x,y;memcpy(&x,&a,4);memcpy(&y,&b,4);return x==y;}return a==b;}
 bool skip(JJAddr a){return io.excluded&&io.excluded(io.ctx,a);}
public:
 JJAddr bytesRead,readFailures;const char *error;
 JJMemoryEngine(JJIO backend):io(backend),scanned(false),bytesRead(0),readFailures(0),error(0){}
 const JJList &getResults()const{return values;}
 JJSize getResultsCount()const{return values.count;}
 bool JJScanMemory(JJRange r,const void *needle,JJType type){
  if(error)return false;
  JJWord value;memcpy(&value,needle,4);JJList next;
  if(scanned){for(JJSize i=0;i<values.count;i++){if(stop()){error="已取消";return false;}JJAddr a=values.data[i];JJWord x;if(a>=r.start&&a<r.end&&io.read(io.ctx,a,&x,4)&&equal(x,value,type)&&!next.append(a)){error="结果过多或内存不足";return false;}}values.swap(next);return true;}
  const JJSize block=256*1024;unsigned char *buffer=(unsigned char*)jj_alloc(block);if(!buffer){error="扫描缓冲区申请失败";return false;}JJAddr cursor=r.start;JJRegion region;bool good=true;
  for(unsigned iter=0;iter<100000;iter++){
   if(stop()){error="已取消或扫描超时";good=false;break;}
   JJAddr queryCursor=cursor;
   if(!io.region(io.ctx,cursor,&region))break;
   if(!region.size||region.start+region.size<=region.start){error="内存区域异常";good=false;break;}
   cursor=region.start+region.size;if(region.start>=r.end)break;if(!(region.protection&2))continue;
   JJAddr lo=region.start>queryCursor?region.start:queryCursor;if(lo<r.start)lo=r.start;JJAddr hi=cursor<r.end?cursor:r.end;if(lo>=hi)continue;
   for(JJAddr at=lo;at<hi;){
    if(stop()){error="已取消或扫描超时";good=false;break;}JJSize n=(JJSize)(hi-at);if(n>block)n=block;
    if(io.read(io.ctx,at,buffer,n)){bytesRead+=n;JJSize offset=(JJSize)((4-(at&3))&3);
     for(;offset+4<=n;offset+=4){JJAddr a=at+offset;JJWord x;memcpy(&x,buffer+offset,4);if(equal(x,value,type)&&!skip(a)&&!next.append(a)){error="结果过多或内存不足";good=false;break;}}
    }else{readFailures++;}
    if(!good)break;
    at+=n;
   }if(!good)break;
   if(iter==99999){error="内存区域数量超限";good=false;}
  }
  jj_free(buffer);if(!good)return false;values.swap(next);scanned=true;return true;
 }
 bool JJNearBySearch(unsigned distance,const void *needle,JJType type){
  if(error)return false;
  JJWord value;memcpy(&value,needle,4);JJList next;
  // Original ARM64 probes show: retain anchors only when a nearby value exists;
  // union anchors and nearby matches; effective 4-byte window [anchor-distance-4, anchor+distance].
  JJSize size=(JJSize)distance*2+8;if(size>1024*1024){error="邻近范围过大";return false;}unsigned char *buffer=(unsigned char*)jj_alloc(size);if(!buffer){error="邻近缓冲区不足";return false;}
  bool good=true;
  for(JJSize i=0;i<values.count;i++){
   if(stop()){error="已取消或扫描超时";good=false;break;}JJAddr anchor=values.data[i],lo=anchor>distance+4?anchor-distance-4:0;JJSize n=size;
   if(!io.read(io.ctx,lo,buffer,n)){
    // Some windows cross an unmapped edge: inspect each aligned value individually.
    bool found=false;for(JJSize j=0;j+4<=n;j+=4){JJWord x;JJAddr a=lo+j;if(!skip(a)&&io.read(io.ctx,a,&x,4)&&equal(x,value,type)){found=true;if(!next.append(a)){good=false;break;}}}if(found&&!next.append(anchor))good=false;
   }else{
    bool found=false;for(JJSize j=(4-(lo&3))&3;j+4<=n;j+=4){JJWord x;JJAddr a=lo+j;memcpy(&x,buffer+j,4);if(!skip(a)&&equal(x,value,type)){found=true;if(!next.append(a)){good=false;break;}}}if(found&&!next.append(anchor))good=false;
   }if(!good){error="结果过多或内存不足";break;}
  }
  jj_free(buffer);if(!good)return false;next.unique();values.swap(next);return true;
 }
 bool JJWriteMemory(JJAddr a,const void *v,JJSize n){return io.write&&io.write(io.ctx,a,v,n);}
 bool readMemory(JJAddr a,void *out,JJSize n){return io.read(io.ctx,a,out,n);}
};
