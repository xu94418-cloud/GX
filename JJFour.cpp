#include "FourFeatures.hpp"
#include "RestoreAll.hpp"
extern "C" {
void *dlsym(void *,const char *);
int vsnprintf(char *,JJSize,const char *,__builtin_va_list);
int snprintf(char *,JJSize,const char *,...);
int strcmp(const char*,const char*);
float strtof(const char*,char**);
void dispatch_async_f(void*,void*,void(*)(void*));
void dispatch_after_f(JJAddr,void*,void*,void(*)(void*));
JJAddr dispatch_time(JJAddr,long long);
void *dispatch_get_global_queue(long,unsigned long);
void *pthread_self(void);
void *pthread_get_stackaddr_np(void*);
JJSize pthread_get_stacksize_np(void*);
}
typedef void *id;typedef void *SEL;struct Rect{double x,y,w,h;};struct Point{double x,y;};struct Size{double w,h;};
static void *msg;static id (*getClass)(const char*);static SEL (*getSel)(const char*);static void *mainq;
template<class T>static T symbol(const char*n){
#ifdef JJFOUR_UI_TEST
 return (T)dlsym((void*)0,n);
#else
 return (T)dlsym((void*)-2,n);
#endif
}
static id cls(const char*n){return getClass(n);}static SEL sel(const char*n){return getSel(n);}
static id send0(id o,const char*s){return ((id(*)(id,SEL))msg)(o,sel(s));}
static id send1(id o,const char*s,id a){return ((id(*)(id,SEL,id))msg)(o,sel(s),a);}
static void setnum(id o,const char*s,long v){((void(*)(id,SEL,long))msg)(o,sel(s),v);}
static void frame(id o,Rect r){((void(*)(id,SEL,Rect))msg)(o,sel("setFrame:"),r);}
static id string(const char*s){return send1(cls("NSString"),"stringWithUTF8String:",(id)s);}
static id make(const char*n){return send0(send0(cls(n),"alloc"),"init");}
static void settitle(id o,const char*s){((void(*)(id,SEL,id,long))msg)(o,sel("setTitle:forState:"),string(s),0);}
static id window,menuCard,menuScroll,actions,header,buttons[4],stateLabels[4],statusLabel,logButton;static bool menuVisible,menuClosing,entryDimmed;
static unsigned menuAnimationSerial,idleSerial;
static id menuBlur,glassTint,rowSpinners[4],customSpinner,floatingSpinner,floatingIcon;
static id presetButtons[4],restoreButton,performanceButton;
static bool performanceMode;
static JJRestoreSummary restoreSummary;
static const unsigned restoreAllJob=5;
static id floatingButton,floatingContainer,titleBar,titleLabel,subtitleLabel,rows[4],nameLabels[4];
static id customRow,customTitle,customHint,customInput,customButton;static float customJobSpeed=4.0f;
static Rect floatingRect={16,96,52,52},menuRect;
static double floatingX=.02,floatingY=.22,menuX=.5,menuY=.5;
static double observedWidth,observedHeight;
#ifdef JJFOUR_UI_TEST
static bool testLandscape=true;
#endif
static Rect snapEntry(Rect,Rect);
static bool layoutPending;static unsigned jobSerial;static unsigned jobFeature;static bool jobEnable;
static int busy,cancelFlag;static JJAddr ownLow,ownHigh,stackLow,stackHigh,startTicks,readBytes;static bool blocked;
static JJFeatureState states[5];static char logs[16000];static JJSize logUsed;
static void logadd(const char*fmt,...){if(logUsed>=sizeof(logs)-1)return;__builtin_va_list a;__builtin_va_start(a,fmt);int n=vsnprintf(logs+logUsed,sizeof(logs)-logUsed,fmt,a);__builtin_va_end(a);if(n>0){JJSize rem=sizeof(logs)-logUsed-1;logUsed+=(JJSize)n>rem?rem:(JJSize)n;}}
static JJAddr (*ticks)();static unsigned numer=1,denom=1;static JJAddr elapsed(){return (ticks()-startTicks)*numer/denom/1000000;}
typedef int(*RegionFn)(JJWord,JJAddr*,JJAddr*,int,int*,JJWord*,JJWord*);
typedef int(*ReadFn)(JJWord,JJAddr,JJAddr,JJAddr,JJAddr*);
typedef int(*WriteFn)(JJWord,JJAddr,JJAddr,JJWord);
static RegionFn regionFn;static ReadFn readFn;static WriteFn writeFn;static int(*deallocFn)(JJWord,JJWord);static JJWord task;static int lastError;
static bool nextregion(void*,JJAddr cursor,JJRegion*out){JJAddr a=cursor,n=0;int info[32]={};JJWord count=32,obj=0;int k=regionFn(task,&a,&n,9,info,&count,&obj);if(obj&&deallocFn)deallocFn(task,obj);if(k)return false;*out={a,n,(JJWord)info[0]};return true;}
static bool readmemory(void*,JJAddr a,void*out,JJSize n){JJAddr got=0;int k=readFn(task,a,n,(JJAddr)out,&got);if(k||got!=n){lastError=k?k:-1;return false;}__atomic_fetch_add(&readBytes,(JJAddr)n,__ATOMIC_RELAXED);return true;}
static bool writememory(void*,JJAddr a,const void*data,JJSize n){int k=writeFn(task,a,(JJAddr)data,(JJWord)n);if(k)lastError=k;return k==0;}
static bool excluded(void*,JJAddr a){if((a>=ownLow&&a<ownHigh)||(a>=stackLow&&a<stackHigh))return true;for(JJBlock*b=jj_blocks;b;b=b->next)if(a>=(JJAddr)b&&a<(JJAddr)(b+1)+b->bytes)return true;return false;}
static bool cancelled(void*){return __atomic_load_n(&cancelFlag,__ATOMIC_RELAXED)||elapsed()>180000;}
static id color(double r,double g,double b,double alpha=1.0){return ((id(*)(id,SEL,double,double,double,double))msg)(cls("UIColor"),sel("colorWithRed:green:blue:alpha:"),r,g,b,alpha);}
static bool reduceMotion(){bool(*fn)()=symbol<bool(*)()>("UIAccessibilityIsReduceMotionEnabled");return fn&&fn();}
static bool reduceTransparency(){bool(*fn)()=symbol<bool(*)()>("UIAccessibilityIsReduceTransparencyEnabled");return fn&&fn();}
static void animateAlpha(id view,double alpha,double duration){if(performanceMode||reduceMotion()||duration<=0){((void(*)(id,SEL,double))msg)(view,sel("setAlpha:"),alpha);return;}id ui=cls("UIView");((void(*)(id,SEL,id,void*))msg)(ui,sel("beginAnimations:context:"),0,0);((void(*)(id,SEL,double))msg)(ui,sel("setAnimationDuration:"),duration);setnum(ui,"setAnimationBeginsFromCurrentState:",1);setnum(ui,"setAnimationCurve:",2);((void(*)(id,SEL,double))msg)(view,sel("setAlpha:"),alpha);send0(ui,"commitAnimations");}
static void idleDim(void*p){if((unsigned)(JJAddr)p!=idleSerial||menuVisible||menuClosing||busy)return;entryDimmed=true;animateAlpha(floatingButton,.58,.35);}
static void wakeFloating(){idleSerial++;if(entryDimmed){entryDimmed=false;animateAlpha(floatingButton,1,.12);}}
static void scheduleDim(){if(menuVisible||menuClosing||busy)return;idleSerial++;dispatch_after_f(dispatch_time(0,4000000000LL),mainq,(void*)(JJAddr)idleSerial,idleDim);}
static void spin(id indicator,bool on){send0(indicator,on&&!performanceMode?"startAnimating":"stopAnimating");}
static id spinner(id tint){id v=((id(*)(id,SEL,long))msg)(send0(cls("UIActivityIndicatorView"),"alloc"),sel("initWithActivityIndicatorStyle:"),100);send1(v,"setColor:",tint);setnum(v,"setHidesWhenStopped:",1);setnum(v,"setUserInteractionEnabled:",0);return v;}
static void border(id view,double width,id tint){id layer=send0(view,"layer");((void(*)(id,SEL,double))msg)(layer,sel("setBorderWidth:"),width);send1(layer,"setBorderColor:",send0(tint,"CGColor"));}
static void radius(id view,double r){((void(*)(id,SEL,double))msg)(send0(view,"layer"),sel("setCornerRadius:"),r);}
static Rect screenRect(){
#ifdef JJFOUR_UI_TEST
 return {0,0,844,390};
#else
 if(window)return ((Rect(*)(id,SEL))msg)(window,sel("bounds"));
 id app=send0(cls("UIApplication"),"sharedApplication"),host=send0(app,"keyWindow");if(!host)host=send0(send0(app,"windows"),"firstObject");if(host)return ((Rect(*)(id,SEL))msg)(host,sel("bounds"));return ((Rect(*)(id,SEL))msg)(send0(cls("UIScreen"),"mainScreen"),sel("bounds"));
#endif
}
static Rect clampRect(Rect r,Rect bounds){double maxX=bounds.w-r.w-8,maxY=bounds.h-r.h-8;if(maxX<8)maxX=8;if(maxY<8)maxY=8;if(r.x<8)r.x=8;if(r.y<8)r.y=8;if(r.x>maxX)r.x=maxX;if(r.y>maxY)r.y=maxY;return r;}
static bool inside(Point p,Rect r){return p.x>=r.x&&p.y>=r.y&&p.x<r.x+r.w&&p.y<r.y+r.h;}
static int hitTarget(Point p){if(inside(p,floatingRect))return 1;if((menuVisible||menuClosing)&&inside(p,menuRect))return 2;return 0;}
static Point convertPoint(id view,Point p,id from){
#ifdef JJFOUR_UI_TEST
 (void)view;(void)from;return p;
#else
 return ((Point(*)(id,SEL,Point,id))msg)(view,sel("convertPoint:fromView:"),p,from);
#endif
}
static id overlayHitTest(id self,SEL,Point p,id event){Point q=convertPoint(floatingContainer,p,self);int target=hitTarget(q);if(target==1)return floatingButton;if(target==2){if(menuClosing)return menuCard;Point local=convertPoint(menuCard,p,self);return ((id(*)(id,SEL,Point,id))msg)(menuCard,sel("hitTest:withEvent:"),local,event);}return 0;}
static double bounded(double x){if(!(x>=0))return 0;if(x>1)return 1;return x;}
static void rememberPosition(bool menu){Rect b=screenRect(),r=menu?menuRect:floatingRect;double x=b.w>r.w?r.x/(b.w-r.w):0,y=b.h>r.h?r.y/(b.h-r.h):0;x=bounded(x);y=bounded(y);if(menu){menuX=x;menuY=y;}else{floatingX=x;floatingY=y;}id defaults=send0(cls("NSUserDefaults"),"standardUserDefaults");((void(*)(id,SEL,double,id))msg)(defaults,sel("setDouble:forKey:"),x,string(menu?"JJFour.menu.x":"JJFour.entry.x"));((void(*)(id,SEL,double,id))msg)(defaults,sel("setDouble:forKey:"),y,string(menu?"JJFour.menu.y":"JJFour.entry.y"));((void(*)(id,SEL,long,id))msg)(defaults,sel("setBool:forKey:"),1,string(menu?"JJFour.menu.saved":"JJFour.entry.saved"));}
static void loadPositions(){id d=send0(cls("NSUserDefaults"),"standardUserDefaults");for(unsigned i=0;i<2;i++){bool menu=i!=0;bool has=((long(*)(id,SEL,id))msg)(d,sel("boolForKey:"),string(menu?"JJFour.menu.saved":"JJFour.entry.saved"));if(!has)continue;double x=((double(*)(id,SEL,id))msg)(d,sel("doubleForKey:"),string(menu?"JJFour.menu.x":"JJFour.entry.x")),y=((double(*)(id,SEL,id))msg)(d,sel("doubleForKey:"),string(menu?"JJFour.menu.y":"JJFour.entry.y"));if(menu){menuX=bounded(x);menuY=bounded(y);}else{floatingX=bounded(x);floatingY=bounded(y);}}}
static void layoutMenu(){Rect screen=screenRect();double w=screen.w-32;if(w>340)w=340;if(w<240)w=240;double h=screen.h-32;if(h>420)h=420;if(h<220)h=220;
 frame(floatingContainer,{0,0,screen.w,screen.h});floatingRect=snapEntry({floatingX*(screen.w-52),floatingY*(screen.h-52),52,52},screen);frame(floatingButton,floatingRect);
 menuRect=clampRect({menuX*(screen.w-w),menuY*(screen.h-h),w,h},screen);frame(menuCard,menuRect);frame(menuBlur,{0,0,w,h});frame(glassTint,{0,0,w,h});frame(titleBar,{0,0,w,72});frame(titleLabel,{20,14,w-110,26});frame(subtitleLabel,{20,42,w-40,20});frame(header,{w-78,14,62,34});
 frame(customRow,{0,0,w-24,96});frame(customTitle,{12,7,94,22});frame(customHint,{12,33,100,18});frame(customInput,{110,16,w-230,32});frame(customButton,{w-108,12,72,36});frame(customSpinner,{w-83,19,22,22});for(unsigned i=0;i<4;i++){double pw=(w-60)/4;frame(presetButtons[i],{12+i*(pw+4),62,pw,26});}
 frame(menuScroll,{12,74,w-24,h-86});((void(*)(id,SEL,Size))msg)(menuScroll,sel("setContentSize:"),(Size){w-24,478});for(unsigned i=0;i<4;i++){frame(rows[i],{0,106+i*54.0,w-24,48});frame(nameLabels[i],{12,4,w-120,22});frame(stateLabels[i],{12,27,w-120,15});frame(buttons[i],{w-90,8,51,31});frame(rowSpinners[i],{w-77,12,24,24});}frame(restoreButton,{0,326,(w-32)/2,38});frame(performanceButton,{(w-32)/2+8,326,(w-32)/2,38});frame(statusLabel,{8,374,w-40,54});frame(logButton,{0,434,w-24,36});}
static bool landscapeReady(Rect r,long orientation){return r.w>0&&r.h>0&&r.w>r.h&&(orientation==0||orientation==3||orientation==4);}
static bool gameLandscape(){
#ifdef JJFOUR_UI_TEST
 return testLandscape;
#else
 if(!window)return false;id scene=send0(window,"windowScene");long o=scene?((long(*)(id,SEL))msg)(scene,sel("interfaceOrientation")):0;return landscapeReady(screenRect(),o);
#endif
}
static void orientationLayout(void*){layoutPending=false;if(!gameLandscape()){send0(customInput,"resignFirstResponder");setnum(floatingContainer,"setHidden:",1);return;}layoutMenu();setnum(floatingContainer,"setHidden:",0);}
static void orientationAction(id,SEL,id){if(layoutPending)return;layoutPending=true;dispatch_after_f(dispatch_time(0,150000000LL),mainq,0,orientationLayout);}
static void overlayLayout(id self,SEL cmd){
#ifndef JJFOUR_UI_TEST
 struct Super {id receiver;id parent;};Super sup={self,cls("UIView")};void(*callSuper)(Super*,SEL)=symbol<void(*)(Super*,SEL)>("objc_msgSendSuper");if(callSuper)callSuper(&sup,cmd);
 Rect r=((Rect(*)(id,SEL))msg)(self,sel("bounds"));if(r.w!=observedWidth||r.h!=observedHeight){observedWidth=r.w;observedHeight=r.h;orientationAction(0,0,0);}
#else
 (void)self;(void)cmd;
#endif
}
static const char *featureName(unsigned f){return f==restoreAllJob?"一键还原":f==4?"自定义攻速":jj_name(f);}
static bool parseSpeed(const char *text,float *out){if(!text)return false;char *end=0;float v=strtof(text,&end);if(end==text)return false;while(*end==' '||*end=='\t'||*end=='\n')end++;if(*end||!(v>0)||(jj_float(v)&0x7f800000U)==0x7f800000U)return false;*out=v;return true;}
static void updateButtons(){
 for(unsigned f=0;f<4;f++){
  ((void(*)(id,SEL,long,long))msg)(buttons[f],sel("setOn:animated:"),states[f].enabled,!performanceMode);setnum(buttons[f],"setEnabled:",!busy&&!blocked&&!(f==0&&states[4].enabled));
  bool pending=busy&&(jobFeature==f||(jobFeature==restoreAllJob&&states[f].enabled));setnum(buttons[f],"setHidden:",pending&&!performanceMode);spin(rowSpinners[f],pending);
  send1(stateLabels[f],"setText:",string(pending?(jobEnable?"查找中…":"还原中…"):states[f].enabled?(states[f].verified?"已开启":"待恢复 · 点击开关重试还原"):"未开启"));send1(stateLabels[f],"setTextColor:",states[f].enabled?(states[f].verified?color(.25,.85,.65):color(1,.65,.35)):color(.52,.58,.67));
 }
 char hint[100];if(states[4].enabled){float v;memcpy(&v,&states[4].activeValue,4);snprintf(hint,sizeof(hint),"已启用 %.6g 倍",(double)v);}else snprintf(hint,sizeof(hint),"与四倍攻速互斥");send1(customHint,"setText:",string(hint));settitle(customButton,states[4].enabled?"还原":"应用");setnum(customButton,"setEnabled:",!busy&&!blocked&&!states[0].enabled);setnum(customInput,"setEnabled:",!busy&&!blocked&&!states[0].enabled&&!states[4].enabled);
 bool customPending=busy&&(jobFeature==4||(jobFeature==restoreAllJob&&states[4].enabled));setnum(customButton,"setHidden:",customPending&&!performanceMode);spin(customSpinner,customPending);if(customPending)send1(customHint,"setText:",string(jobEnable?"查找中…":"还原中…"));
 bool floatPending=busy&&!performanceMode;spin(floatingSpinner,floatPending);((void(*)(id,SEL,id,long))msg)(floatingButton,sel("setImage:forState:"),floatPending?0:floatingIcon,0);settitle(floatingButton,floatPending||floatingIcon?"":"JJ");
 bool any=false;for(unsigned i=0;i<5;i++)if(states[i].enabled||states[i].count)any=true;
 settitle(restoreButton,busy&&jobFeature==restoreAllJob?"正在还原…":"一键还原");setnum(restoreButton,"setEnabled:",!busy&&!blocked&&any);settitle(performanceButton,performanceMode?"性能模式：开":"性能模式：关");setnum(performanceButton,"setEnabled:",!busy);for(unsigned i=0;i<4;i++)setnum(presetButtons[i],"setEnabled:",!busy&&!blocked&&!states[0].enabled&&!states[4].enabled);
 settitle(header,busy?(jobEnable?"取消查找":"正在还原"):"关闭");setnum(logButton,"setEnabled:",!busy);
}
static void applyPerformanceStyle(){bool solid=performanceMode||reduceTransparency();if(menuBlur){id effect=solid?0:((id(*)(id,SEL,long))msg)(cls("UIBlurEffect"),sel("effectWithStyle:"),17);send1(menuBlur,"setEffect:",effect);setnum(menuBlur,"setHidden:",solid);}send1(menuCard,"setBackgroundColor:",solid||!menuBlur?color(.018,.038,.075,.98):send0(cls("UIColor"),"clearColor"));((void(*)(id,SEL,float))msg)(send0(floatingButton,"layer"),sel("setShadowOpacity:"),performanceMode?0.0f:.42f);}
static void performanceAction(id,SEL,id){if(busy||menuClosing)return;performanceMode=!performanceMode;id d=send0(cls("NSUserDefaults"),"standardUserDefaults");((void(*)(id,SEL,long,id))msg)(d,sel("setBool:forKey:"),performanceMode,string("JJFour.performance"));applyPerformanceStyle();updateButtons();send1(statusLabel,"setText:",string(performanceMode?"性能模式已开启：实色背景、减少动画":"性能模式已关闭：恢复毛玻璃与动画"));}
static void finish(void*){
 busy=0;if(jobFeature==restoreAllJob){char text[240];snprintf(text,sizeof(text),"还原%lu处 · 跳过%lu处 · 失败%lu处\n未完成%lu项，可再次重试",restoreSummary.restored,restoreSummary.skipped,restoreSummary.failed,restoreSummary.remainingFeatures);send1(statusLabel,"setText:",string(text));send1(statusLabel,"setTextColor:",restoreSummary.remainingFeatures?color(1,.55,.48):restoreSummary.skipped?color(.98,.76,.4):color(.34,.86,.68));updateButtons();scheduleDim();logadd("\n一键还原：尝试%lu项，还原%lu处，跳过%lu处，失败%lu处，未完成%lu项\n",restoreSummary.attemptedFeatures,restoreSummary.restored,restoreSummary.skipped,restoreSummary.failed,restoreSummary.remainingFeatures);for(unsigned i=0;i<5;i++)if(restoreSummary.attemptedMask&(1U<<i))logadd("  %s：还原%lu，跳过%lu，失败%lu\n",featureName(i),states[i].report.restored,states[i].report.skipped,states[i].report.failed);return;}JJFeatureState&s=states[jobFeature];JJFeatureReport&r=s.report;char text[240];
 if(r.error)snprintf(text,sizeof(text),"%s：%s",featureName(jobFeature),r.error);
 else if(jobEnable)snprintf(text,sizeof(text),"%s：已写入 %lu 处",featureName(jobFeature),r.written);
 else snprintf(text,sizeof(text),"%s：还原 %lu 处，跳过 %lu 处",featureName(jobFeature),r.restored,r.skipped);
 send1(statusLabel,"setText:",string(text));send1(statusLabel,"setTextColor:",r.error?color(1,.55,.48):color(.34,.86,.68));updateButtons();scheduleDim();
 logadd("\n%s / %s\n初次=%lu 联合1=%lu 联合2=%lu 精确=%lu\n准备=%lu 写入=%lu 失败=%lu 还原=%lu 跳过=%lu\n扫描读入=%llu MiB 读取失败=%llu 最后Mach错误=%d\n结果=%s\n",featureName(jobFeature),jobEnable?"开启":"还原",r.initial,r.joint1,r.joint2,r.refined,r.prepared,r.written,r.failed,r.restored,r.skipped,r.bytesRead/1024/1024,r.readFailures,lastError,r.error?r.error:"完成");
 if(jobFeature==4){float value=customJobSpeed;if(!jobEnable)memcpy(&value,&s.activeValue,4);logadd("自定义倍率=%.9g\n",(double)value);}
 if(s.count){logadd("记录地址（最多8处）:\n");for(JJSize j=0;j<s.count&&j<8;j++)logadd("  0x%llX\n",s.records[j].address);}
}
static void worker(void*){
 void*th=pthread_self();stackHigh=(JJAddr)pthread_get_stackaddr_np(th);stackLow=stackHigh-pthread_get_stacksize_np(th);startTicks=ticks();__atomic_store_n(&readBytes,0,__ATOMIC_RELAXED);lastError=0;
 JJIO io={0,nextregion,readmemory,writememory,excluded,cancelled};if(jobFeature==restoreAllJob)jj_restore_all(io,states,restoreSummary);else jj_apply_feature(io,jobFeature==4?JJ_SPEED:jobFeature,jobEnable,states[jobFeature],jobFeature==4?customJobSpeed:4.0f);dispatch_async_f(mainq,0,finish);
}
static void progressTick(void*p){if(!busy||(unsigned)(JJAddr)p!=jobSerial)return;char text[180];JJAddr bytes=__atomic_load_n(&readBytes,__ATOMIC_RELAXED);if(!jobEnable)snprintf(text,sizeof(text),"正在还原，请稍候");else if(__atomic_load_n(&cancelFlag,__ATOMIC_RELAXED))snprintf(text,sizeof(text),"正在取消 · 已读取 %llu MiB",bytes/1024/1024);else snprintf(text,sizeof(text),"正在查找 · 已读取 %llu MiB\n右上角可取消",bytes/1024/1024);send1(statusLabel,"setText:",string(text));dispatch_after_f(dispatch_time(0,performanceMode?800000000LL:300000000LL),mainq,p,progressTick);}
static void beginJob(unsigned f){
 if(!regionFn||!readFn||!writeFn||!task||!ticks){send1(statusLabel,"setText:",string("系统接口不可用，未执行修改"));updateButtons();return;}
 wakeFloating();busy=1;jobSerial++;__atomic_store_n(&readBytes,0,__ATOMIC_RELAXED);__atomic_store_n(&cancelFlag,0,__ATOMIC_RELAXED);jobFeature=(unsigned)f;jobEnable=f==restoreAllJob?false:!states[f].enabled;updateButtons();send1(statusLabel,"setText:",string(jobEnable?"正在查找，可点右上角取消":"正在还原"));send1(statusLabel,"setTextColor:",color(.65,.76,.95));dispatch_async_f(dispatch_get_global_queue(-2,0),0,worker);dispatch_after_f(dispatch_time(0,performanceMode?800000000LL:300000000LL),mainq,(void*)(JJAddr)jobSerial,progressTick);
}
static void featureAction(id,SEL,id sender){
 if(busy||blocked||menuClosing){updateButtons();return;}long f=((long(*)(id,SEL))msg)(sender,sel("tag"));if(f<0||f>3)return;if(f==0&&states[4].enabled){send1(statusLabel,"setText:",string("请先还原自定义攻速"));updateButtons();return;}beginJob((unsigned)f);
}
static void restoreAllAction(id,SEL,id){if(busy||blocked||menuClosing)return;bool any=false;for(unsigned i=0;i<5;i++)if(states[i].enabled||states[i].count)any=true;if(!any){send1(statusLabel,"setText:",string("没有需要还原的功能"));return;}send0(customInput,"resignFirstResponder");beginJob(restoreAllJob);}
static void presetAction(id,SEL,id sender){if(busy||blocked||menuClosing)return;if(states[0].enabled||states[4].enabled){send1(statusLabel,"setText:",string("请先还原攻速，再选择倍率"));return;}long i=((long(*)(id,SEL))msg)(sender,sel("tag"));if(i<0||i>3)return;static const char *values[]={"1","2","4","8"};send0(customInput,"resignFirstResponder");send1(customInput,"setText:",string(values[i]));send1(statusLabel,"setText:",string("倍率已填入，点击应用后生效"));}
static void customSpeedAction(id,SEL,id){
 if(busy||blocked||menuClosing)return;send0(customInput,"resignFirstResponder");if(states[0].enabled){send1(statusLabel,"setText:",string("请先关闭四倍攻速"));updateButtons();return;}
 if(!states[4].enabled){id text=send0(customInput,"text");text=((id(*)(id,SEL,id,id))msg)(text,sel("stringByReplacingOccurrencesOfString:withString:"),string(","),string("."));const char *raw=(const char*)send0(text,"UTF8String");float v;if(!parseSpeed(raw,&v)){send1(statusLabel,"setText:",string("请输入大于0的有效倍率，例如2.5"));return;}customJobSpeed=v;id d=send0(cls("NSUserDefaults"),"standardUserDefaults");((void(*)(id,SEL,double,id))msg)(d,sel("setDouble:forKey:"),(double)v,string("JJFour.customSpeed"));}
 beginJob(4);
}
static void finishMenuClose(void*p){if((unsigned)(JJAddr)p!=menuAnimationSerial||menuVisible)return;menuClosing=false;setnum(menuCard,"setHidden:",1);scheduleDim();}
static void closeMenu(){send0(customInput,"resignFirstResponder");menuVisible=false;menuClosing=true;menuAnimationSerial++;animateAlpha(menuCard,0,.12);dispatch_after_f(dispatch_time(0,performanceMode||reduceMotion()?0:140000000LL),mainq,(void*)(JJAddr)menuAnimationSerial,finishMenuClose);}
static void toggleMenuAction(id,SEL,id){wakeFloating();if(menuVisible){closeMenu();return;}menuVisible=true;menuClosing=false;menuAnimationSerial++;layoutMenu();setnum(menuCard,"setHidden:",0);((void(*)(id,SEL,double))msg)(menuCard,sel("setAlpha:"),0.0);animateAlpha(menuCard,1,.16);}
static void headerAction(id,SEL,id){
 if(busy){if(!jobEnable){send1(statusLabel,"setText:",string("正在还原，请稍候"));return;}__atomic_store_n(&cancelFlag,1,__ATOMIC_RELAXED);send1(statusLabel,"setText:",string("正在取消，请稍候"));return;}
 closeMenu();
}
static Point panDelta(id gesture){
#ifdef JJFOUR_UI_TEST
 (void)gesture;return {20,30};
#else
 return ((Point(*)(id,SEL,id))msg)(gesture,sel("translationInView:"),floatingContainer);
#endif
}
static Rect snapEntry(Rect r,Rect bounds){r=clampRect(r,bounds);r.x=(r.x+r.w/2<bounds.w/2)?8:bounds.w-r.w-8;return clampRect(r,bounds);}
static void moveView(id gesture,bool menu){wakeFloating();Point p=panDelta(gesture);Rect r=menu?menuRect:floatingRect;r.x+=p.x;r.y+=p.y;r=clampRect(r,screenRect());if(menu){menuRect=r;frame(menuCard,r);}else{floatingRect=r;frame(floatingButton,r);}((void(*)(id,SEL,Point,id))msg)(gesture,sel("setTranslation:inView:"),(Point){0,0},floatingContainer);long state=((long(*)(id,SEL))msg)(gesture,sel("state"));if(state==3||state==4){if(!menu){floatingRect=snapEntry(floatingRect,screenRect());frame(floatingButton,floatingRect);}rememberPosition(menu);scheduleDim();}}
static void dragAction(id,SEL,id gesture){moveView(gesture,false);}
static void menuDragAction(id,SEL,id gesture){moveView(gesture,true);}
static void logAction(id,SEL,id){if(busy)return;send1(send0(cls("UIPasteboard"),"generalPasteboard"),"setString:",string(logs));send1(statusLabel,"setText:",string("记录已取出，可粘贴到对话"));}
static id makeButton(const char*text,const char*action,Rect r){id b=((id(*)(id,SEL,long))msg)(cls("UIButton"),sel("buttonWithType:"),1);frame(b,r);settitle(b,text);((void(*)(id,SEL,id,SEL,JJAddr))msg)(b,sel("addTarget:action:forControlEvents:"),actions,sel(action),1ULL<<6);return b;}
static void setup(void*p){
 unsigned tries=(unsigned)(JJAddr)p;if(!cls("UIApplication")||!cls("UIWindow")){if(tries<12)dispatch_after_f(dispatch_time(0,3000000000LL),mainq,(void*)(JJAddr)(tries+1),setup);return;}
 id app=send0(cls("UIApplication"),"sharedApplication"),host=send0(app,"keyWindow");if(!host)host=send0(send0(app,"windows"),"firstObject");id scene=send0(host,"windowScene");Rect hostBounds=host?((Rect(*)(id,SEL))msg)(host,sel("bounds")):(Rect){0,0,0,0};long hostOrientation=scene?((long(*)(id,SEL))msg)(scene,sel("interfaceOrientation")):0;
 if(!host||!landscapeReady(hostBounds,hostOrientation)){dispatch_after_f(dispatch_time(0,1000000000LL),mainq,0,setup);return;}
 id pool=make("NSAutoreleasePool");id prefs=send0(cls("NSUserDefaults"),"standardUserDefaults");performanceMode=((long(*)(id,SEL,id))msg)(prefs,sel("boolForKey:"),string("JJFour.performance"));id(*allocate)(id,const char*,JJSize)=symbol<id(*)(id,const char*,JJSize)>("objc_allocateClassPair");void(*reg)(id)=symbol<void(*)(id)>("objc_registerClassPair");int(*method)(id,SEL,void*,const char*)=symbol<int(*)(id,SEL,void*,const char*)>("class_addMethod");
 id ac=allocate(cls("NSObject"),"JJFourStandaloneActions",0);if(!ac){send0(pool,"drain");return;}method(ac,sel("feature:"),(void*)featureAction,"v@:@");method(ac,sel("header:"),(void*)headerAction,"v@:@");method(ac,sel("log:"),(void*)logAction,"v@:@");method(ac,sel("toggle:"),(void*)toggleMenuAction,"v@:@");method(ac,sel("drag:"),(void*)dragAction,"v@:@");method(ac,sel("menuDrag:"),(void*)menuDragAction,"v@:@");method(ac,sel("orientation:"),(void*)orientationAction,"v@:@");method(ac,sel("customSpeed:"),(void*)customSpeedAction,"v@:@");method(ac,sel("restoreAll:"),(void*)restoreAllAction,"v@:@");method(ac,sel("preset:"),(void*)presetAction,"v@:@");method(ac,sel("performance:"),(void*)performanceAction,"v@:@");reg(ac);actions=make("JJFourStandaloneActions");
 unsigned(*countImages)()=symbol<unsigned(*)()>("_dyld_image_count");const char*(*imageName)(unsigned)=symbol<const char*(*)(unsigned)>("_dyld_get_image_name");
 if(countImages&&imageName){for(unsigned i=0;i<countImages();i++){const char*n=imageName(i),*base=n;if(!n)continue;for(const char*t=n;*t;t++)if(*t=='/')base=t+1;if(!strcmp(base,"YSH.dylib")||!strcmp(base,"YSH-range16.dylib"))blocked=true;}}
 if(cls("MyMenu")&&cls("SpeedUDIDHelper"))blocked=true;
 id overlayClass=allocate(cls("UIView"),"JJFourPassthroughView",0);if(!overlayClass){send0(pool,"drain");return;}method(overlayClass,sel("hitTest:withEvent:"),(void*)overlayHitTest,"@@:{CGPoint=dd}@");method(overlayClass,sel("layoutSubviews"),(void*)overlayLayout,"v@:");reg(overlayClass);
 window=send0(host,"retain");Rect screen=hostBounds;floatingContainer=((id(*)(id,SEL,Rect))msg)(send0(overlayClass,"alloc"),sel("initWithFrame:"),(Rect){0,0,screen.w,screen.h});send1(floatingContainer,"setBackgroundColor:",send0(cls("UIColor"),"clearColor"));setnum(floatingContainer,"setAutoresizingMask:",18);send1(host,"addSubview:",floatingContainer);
 menuCard=make("UIView");send1(menuCard,"setBackgroundColor:",color(.018,.038,.075,.97));radius(menuCard,22);setnum(menuCard,"setClipsToBounds:",1);border(menuCard,1,color(.08,.82,1,.65));send1(floatingContainer,"addSubview:",menuCard);
 if(!reduceTransparency()&&cls("UIBlurEffect")&&cls("UIVisualEffectView")){id effect=performanceMode?0:((id(*)(id,SEL,long))msg)(cls("UIBlurEffect"),sel("effectWithStyle:"),17);menuBlur=((id(*)(id,SEL,id))msg)(send0(cls("UIVisualEffectView"),"alloc"),sel("initWithEffect:"),effect);setnum(menuBlur,"setAutoresizingMask:",18);setnum(menuBlur,"setUserInteractionEnabled:",0);send1(menuCard,"addSubview:",menuBlur);send1(menuCard,"setBackgroundColor:",send0(cls("UIColor"),"clearColor"));glassTint=make("UIView");send1(glassTint,"setBackgroundColor:",color(.035,.055,.10,.48));setnum(glassTint,"setAutoresizingMask:",0);send1(send0(menuBlur,"contentView"),"addSubview:",glassTint);}
 titleBar=make("UIView");send1(menuCard,"addSubview:",titleBar);
 titleLabel=make("UILabel");send1(titleLabel,"setText:",string("JJ · 功能面板"));send1(titleLabel,"setTextColor:",color(.95,.97,1));send1(titleLabel,"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("boldSystemFontOfSize:"),18.0));send1(titleBar,"addSubview:",titleLabel);
 subtitleLabel=make("UILabel");send1(subtitleLabel,"setText:",string("横屏专属 · 拖动标题栏移动"));send1(subtitleLabel,"setTextColor:",color(.52,.60,.72));send1(subtitleLabel,"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),12.0));send1(titleBar,"addSubview:",subtitleLabel);
 header=makeButton("关闭","header:",{0,0,62,34});send1(titleBar,"addSubview:",header);
 id menuPan=((id(*)(id,SEL,id,SEL))msg)(send0(cls("UIPanGestureRecognizer"),"alloc"),sel("initWithTarget:action:"),actions,sel("menuDrag:"));setnum(menuPan,"setCancelsTouchesInView:",1);setnum(menuPan,"setMaximumNumberOfTouches:",1);send1(titleBar,"addGestureRecognizer:",menuPan);
 menuScroll=make("UIScrollView");setnum(menuScroll,"setShowsVerticalScrollIndicator:",0);send1(menuCard,"addSubview:",menuScroll);
 customRow=make("UIView");send1(customRow,"setBackgroundColor:",color(.04,.10,.18));radius(customRow,12);send1(menuScroll,"addSubview:",customRow);
 customTitle=make("UILabel");send1(customTitle,"setText:",string("自定义攻速"));send1(customTitle,"setTextColor:",color(.92,.95,1));send1(customTitle,"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),15.0));send1(customRow,"addSubview:",customTitle);
 customHint=make("UILabel");send1(customHint,"setTextColor:",color(.52,.60,.72));send1(customHint,"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),10.0));send1(customRow,"addSubview:",customHint);
 customInput=make("UITextField");send1(customInput,"setBackgroundColor:",color(.055,.075,.12));send1(customInput,"setTextColor:",color(.95,.97,1));setnum(customInput,"setKeyboardType:",8);setnum(customInput,"setTextAlignment:",1);setnum(customInput,"setAutocorrectionType:",1);radius(customInput,8);send1(customInput,"setPlaceholder:",string("倍率"));id defaults=send0(cls("NSUserDefaults"),"standardUserDefaults");double saved=((double(*)(id,SEL,id))msg)(defaults,sel("doubleForKey:"),string("JJFour.customSpeed"));float last=(float)saved;if(!(last>0)||(jj_float(last)&0x7f800000U)==0x7f800000U)last=4;char initial[40];snprintf(initial,sizeof(initial),"%.7g",(double)last);send1(customInput,"setText:",string(initial));send1(customRow,"addSubview:",customInput);
 const char *presetNames[]={"1×","2×","4×","8×"};for(unsigned i=0;i<4;i++){presetButtons[i]=makeButton(presetNames[i],"preset:",{0,0,60,26});setnum(presetButtons[i],"setTag:",i);send1(presetButtons[i],"setBackgroundColor:",color(.1,.22,.32,.7));radius(presetButtons[i],7);send1(customRow,"addSubview:",presetButtons[i]);}
 customButton=makeButton("应用","customSpeed:",{0,0,72,36});send1(customButton,"setBackgroundColor:",color(.1,.35,.48,.6));radius(customButton,9);send1(customRow,"addSubview:",customButton);customSpinner=spinner(color(.35,.91,.77));send1(customRow,"addSubview:",customSpinner);
 for(unsigned i=0;i<4;i++){
  rows[i]=make("UIView");send1(rows[i],"setBackgroundColor:",color(.035,.095,.17,.80));radius(rows[i],12);border(rows[i],.6,color(.1,.72,1,.38));send1(menuScroll,"addSubview:",rows[i]);
  nameLabels[i]=make("UILabel");send1(nameLabels[i],"setText:",string(jj_name(i)));send1(nameLabels[i],"setTextColor:",color(.92,.95,1));send1(nameLabels[i],"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),15.0));send1(rows[i],"addSubview:",nameLabels[i]);
  stateLabels[i]=make("UILabel");send1(stateLabels[i],"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),10.0));send1(rows[i],"addSubview:",stateLabels[i]);
  buttons[i]=make("UISwitch");setnum(buttons[i],"setTag:",i);send1(buttons[i],"setOnTintColor:",color(.04,.65,1.));((void(*)(id,SEL,id,SEL,JJAddr))msg)(buttons[i],sel("addTarget:action:forControlEvents:"),actions,sel("feature:"),1ULL<<12);send1(rows[i],"addSubview:",buttons[i]);rowSpinners[i]=spinner(color(.35,.91,.77));send1(rows[i],"addSubview:",rowSpinners[i]);
 }
 restoreButton=makeButton("一键还原","restoreAll:",{0,0,140,38});send1(restoreButton,"setBackgroundColor:",color(.06,.22,.42,.88));radius(restoreButton,10);send1(menuScroll,"addSubview:",restoreButton);
 performanceButton=makeButton("性能模式：关","performance:",{0,0,140,38});send1(performanceButton,"setBackgroundColor:",color(.16,.19,.29,.72));radius(performanceButton,10);send1(menuScroll,"addSubview:",performanceButton);
 statusLabel=make("UILabel");setnum(statusLabel,"setNumberOfLines:",3);send1(statusLabel,"setTextColor:",color(.65,.73,.85));send1(statusLabel,"setFont:",((id(*)(id,SEL,double))msg)(cls("UIFont"),sel("systemFontOfSize:"),12.0));send1(statusLabel,"setText:",string(blocked?"检测到原YSH，请移除它后重新签名":"进入游戏场景后，逐项开启"));send1(menuScroll,"addSubview:",statusLabel);
 logButton=makeButton("取出运行记录","log:",{0,0,270,36});send1(logButton,"setBackgroundColor:",color(.04,.17,.34));radius(logButton,10);send1(menuScroll,"addSubview:",logButton);
 floatingButton=makeButton("JJ","toggle:",{0,0,52,52});send1(floatingButton,"setBackgroundColor:",color(.16,.36,.95));((void(*)(id,SEL,id,long))msg)(floatingButton,sel("setTitleColor:forState:"),color(1,1,1),0);radius(floatingButton,26);border(floatingButton,1.2,color(.8,.94,1,.55));id entryLayer=send0(floatingButton,"layer");send1(entryLayer,"setShadowColor:",send0(color(.02,.12,.27),"CGColor"));((void(*)(id,SEL,float))msg)(entryLayer,sel("setShadowOpacity:"),.42f);((void(*)(id,SEL,double))msg)(entryLayer,sel("setShadowRadius:"),8.0);((void(*)(id,SEL,Size))msg)(entryLayer,sel("setShadowOffset:"),(Size){0,3});
 if(cls("CAGradientLayer")){id gradient=make("CAGradientLayer");frame(gradient,{0,0,52,52});((void(*)(id,SEL,double))msg)(gradient,sel("setCornerRadius:"),26.0);id colors[]={send0(color(.16,.36,.96),"CGColor"),send0(color(.06,.7,.74),"CGColor")};id array=((id(*)(id,SEL,const id*,JJSize))msg)(cls("NSArray"),sel("arrayWithObjects:count:"),colors,2);send1(gradient,"setColors:",array);((void(*)(id,SEL,Point))msg)(gradient,sel("setStartPoint:"),(Point){.1,0});((void(*)(id,SEL,Point))msg)(gradient,sel("setEndPoint:"),(Point){.9,1});((void(*)(id,SEL,id,unsigned))msg)(entryLayer,sel("insertSublayer:atIndex:"),gradient,0);}
 floatingIcon=send1(cls("UIImage"),"systemImageNamed:",string("bolt.fill"));if(floatingIcon){id configuration=((id(*)(id,SEL,double,long))msg)(cls("UIImageSymbolConfiguration"),sel("configurationWithPointSize:weight:"),23.0,6);floatingIcon=send1(floatingIcon,"imageWithConfiguration:",configuration);send1(floatingButton,"setTintColor:",color(.95,1,1));}floatingSpinner=spinner(color(1,1,1));frame(floatingSpinner,{14,14,24,24});send1(floatingButton,"addSubview:",floatingSpinner);send1(floatingContainer,"addSubview:",floatingButton);
 id pan=((id(*)(id,SEL,id,SEL))msg)(send0(cls("UIPanGestureRecognizer"),"alloc"),sel("initWithTarget:action:"),actions,sel("drag:"));setnum(pan,"setCancelsTouchesInView:",1);setnum(pan,"setMaximumNumberOfTouches:",1);send1(floatingButton,"addGestureRecognizer:",pan);
 loadPositions();layoutMenu();menuVisible=false;applyPerformanceStyle();updateButtons();setnum(menuCard,"setHidden:",1);setnum(floatingContainer,"setHidden:",0);scheduleDim();
 id center=send0(cls("NSNotificationCenter"),"defaultCenter");((void(*)(id,SEL,id,SEL,id,id))msg)(center,sel("addObserver:selector:name:object:"),actions,sel("orientation:"),string("UIDeviceOrientationDidChangeNotification"),0);send0(send0(cls("UIDevice"),"currentDevice"),"beginGeneratingDeviceOrientationNotifications");
 logadd("JJFour 独立实现 v9（快捷倍率·一键还原·性能模式）\nJJMemoryEngine兼容实现，非原作者源码。\n本地功能配置；搜索上限×16。无UDID、无网络认证。\n攻速判断/写入偏移：+20（十进制，即+0x14）。\n");send0(pool,"drain");
}
struct DlInfo{const char*file;void*base;const char*name;void*address;};
__attribute__((constructor))static void start(){
 msg=dlsym((void*)-2,"objc_msgSend");getClass=symbol<id(*)(const char*)>("objc_getClass");getSel=symbol<SEL(*)(const char*)>("sel_registerName");mainq=dlsym((void*)-2,"_dispatch_main_q");
 regionFn=symbol<RegionFn>("mach_vm_region");readFn=symbol<ReadFn>("vm_read_overwrite");writeFn=symbol<WriteFn>("vm_write");deallocFn=symbol<int(*)(JJWord,JJWord)>("mach_port_deallocate");JJWord*t=symbol<JJWord*>("mach_task_self_");if(t)task=*t;
 ticks=symbol<JJAddr(*)()>("mach_continuous_time");int(*timebase)(unsigned*)=symbol<int(*)(unsigned*)>("mach_timebase_info");unsigned tb[2]={1,1};if(timebase&&!timebase(tb)&&tb[1]){numer=tb[0];denom=tb[1];}
 int(*addressInfo)(const void*,DlInfo*)=symbol<int(*)(const void*,DlInfo*)>("dladdr");DlInfo info={};if(addressInfo&&addressInfo((const void*)start,&info)&&info.base){unsigned char*h=(unsigned char*)info.base;JJWord n;memcpy(&n,h+16,4);JJSize off=32;JJAddr slide=0;for(JJWord i=0;i<n;i++){JJWord cmd,size;memcpy(&cmd,h+off,4);memcpy(&size,h+off+4,4);if(size<8)break;if(cmd==0x19&&h[off+10]=='T'){JJAddr vm;memcpy(&vm,h+off+24,8);slide=(JJAddr)h-vm;}off+=size;}off=32;ownLow=(JJAddr)h;ownHigh=ownLow;for(JJWord i=0;i<n;i++){JJWord cmd,size;memcpy(&cmd,h+off,4);memcpy(&size,h+off+4,4);if(size<8)break;if(cmd==0x19){JJAddr vm,len;memcpy(&vm,h+off+24,8);memcpy(&len,h+off+32,8);if(vm+slide+len>ownHigh)ownHigh=vm+slide+len;}off+=size;}}
 if(msg&&getClass&&getSel&&mainq)dispatch_after_f(dispatch_time(0,10000000000LL),mainq,0,setup);
}
