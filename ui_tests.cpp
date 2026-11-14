#define __attribute__(x)
#define JJFOUR_UI_TEST 1
#include "JJFour.cpp"
extern "C" int printf(const char*,...);
static unsigned queued,badSender,clipboardWrites,timers,windowMoves,panResets;static long gestureState=2,senderTag=3;static const char *customText="2.5";static const char *filledText=0;static bool popupHidden=true,containerHidden=false;static JJAddr fakeTicks(){return 0;}
static id fakeClass(const char*s){return (id)s;}static SEL fakeSel(const char*s){return (SEL)s;}
static id fakeMsg(id obj,SEL method,...){const char*s=(const char*)method;__builtin_va_list a;__builtin_va_start(a,method);id out=0;
 if(!strcmp(s,"stringWithUTF8String:"))out=__builtin_va_arg(a,id);
 else if(!strcmp(s,"tag")){if(obj!=(id)0x200)badSender++;out=(id)senderTag;}
 else if(!strcmp(s,"setString:"))clipboardWrites++;
 else if(!strcmp(s,"setText:")&&obj==(id)0xa00)filledText=__builtin_va_arg(a,const char*);
 else if(!strcmp(s,"text")&&obj==(id)0xa00)out=(id)customText;
 else if(!strcmp(s,"stringByReplacingOccurrencesOfString:withString:"))out=obj;
 else if(!strcmp(s,"UTF8String"))out=obj;
 else if(!strcmp(s,"setHidden:")&&obj==(id)0x300)popupHidden=__builtin_va_arg(a,long)!=0;
 else if(!strcmp(s,"setHidden:")&&obj==(id)0x700)containerHidden=__builtin_va_arg(a,long)!=0;
 else if(!strcmp(s,"generalPasteboard"))out=(id)0x500;
 else if(!strcmp(s,"state"))out=(id)gestureState;
 else if(!strcmp(s,"setFrame:")&&obj==(id)0x800)windowMoves++;
 else if(!strcmp(s,"setTranslation:inView:"))panResets++;
 else if(!strcmp(s,"hitTest:withEvent:"))out=(id)0x900;
 __builtin_va_end(a);return out;}
extern "C" void dispatch_async_f(void*,void*,void(*)(void*)){queued++;}
extern "C" void dispatch_after_f(JJAddr,void*,void*,void(*)(void*)){timers++;}
extern "C" JJAddr dispatch_time(JJAddr t,long long n){return t+(JJAddr)n;}
extern "C" void *dispatch_get_global_queue(long,unsigned long){return (void*)1;}
int main(){msg=(void*)fakeMsg;getClass=fakeClass;getSel=fakeSel;regionFn=(RegionFn)1;readFn=(ReadFn)1;writeFn=(WriteFn)1;task=1;ticks=fakeTicks;
 featureAction((id)0x100,(SEL)"feature:",(id)0x200);
 if(badSender||queued!=1||!busy||jobFeature!=3||!jobEnable)return 1;
 featureAction((id)0x100,0,(id)0x200);if(queued!=1)return 2;
 logAction(0,0,0);if(clipboardWrites)return 3;
 headerAction(0,0,0);if(!cancelFlag)return 4;
 busy=0;logAction(0,0,0);if(clipboardWrites!=1)return 5;
 menuCard=(id)0x300;menuScroll=(id)0x400;menuVisible=false;unsigned oldQueued=queued;
 toggleMenuAction(0,0,0);if(!menuVisible||popupHidden||queued!=oldQueued)return 6;
 headerAction(0,0,0);if(menuVisible||!menuClosing||queued!=oldQueued)return 7;finishMenuClose((void*)(JJAddr)menuAnimationSerial);if(!popupHidden)return 7;
 toggleMenuAction(0,0,0);toggleMenuAction(0,0,0);finishMenuClose((void*)(JJAddr)menuAnimationSerial);if(menuVisible||!popupHidden||queued!=oldQueued)return 8;
 // Independent drags move subviews, not the full-screen window.
 window=(id)0x800;floatingButton=(id)0x600;floatingContainer=(id)0x700;menuCard=(id)0x300;
 floatingRect={20,40,52,52};menuRect={200,60,340,300};unsigned initialWindowMoves=windowMoves;
 dragAction(0,0,(id)0x1000);if(floatingRect.x!=40||floatingRect.y!=70||menuRect.x!=200||windowMoves!=initialWindowMoves)return 9;
 menuDragAction(0,0,(id)0x1000);if(menuRect.x!=220||menuRect.y!=82||floatingRect.x!=40||windowMoves!=initialWindowMoves||panResets!=2)return 10;
 Rect edge=clampRect({-100,9999,52,52},{0,0,844,390});if(edge.x!=8||edge.y!=330)return 11;
 menuVisible=false;if(hitTarget({45,75})!=1||hitTarget({300,150})!=0)return 12;
 menuVisible=true;if(hitTarget({300,150})!=2||hitTarget({800,10})!=0)return 13;
 if(overlayHitTest(window,0,{45,75},0)!=floatingButton||overlayHitTest(window,0,{300,150},0)!=(id)0x900||overlayHitTest(window,0,{800,10},0)!=0)return 14;
 gestureState=3;dragAction(0,0,(id)0x1000);if(!(floatingX>=0&&floatingX<=1&&floatingY>=0&&floatingY<=1))return 15;
 unsigned oldTimers=timers;busy=0;progressTick((void*)(JJAddr)jobSerial);if(timers!=oldTimers)return 16;
 if(!landscapeReady({0,0,844,390},3)||!landscapeReady({0,0,844,390},4)||!landscapeReady({0,0,844,390},0))return 17;
 if(landscapeReady({0,0,390,844},1)||landscapeReady({0,0,844,390},1)||landscapeReady({0,0,0,0},3))return 18;
 unsigned priorHostMoves=windowMoves;testLandscape=false;orientationLayout(0);if(!containerHidden||windowMoves!=priorHostMoves)return 19;
 testLandscape=true;orientationLayout(0);if(containerHidden||windowMoves!=priorHostMoves)return 20;
 unsigned priorTimers=timers;setup(0);if(timers!=priorTimers+1)return 21;
 float parsed=0;if(!parseSpeed(" 2.5 \n",&parsed)||parsed!=2.5f||parseSpeed("0",&parsed)||parseSpeed("-1",&parsed)||parseSpeed("nan",&parsed)||parseSpeed("inf",&parsed)||parseSpeed("2.5x",&parsed)||parseSpeed("",&parsed))return 22;
 busy=0;blocked=false;states[0].enabled=true;unsigned beforeCustom=queued;customSpeedAction(0,0,0);if(queued!=beforeCustom)return 23;states[0].enabled=false;
 states[4].enabled=true;senderTag=0;featureAction(0,0,(id)0x200);if(queued!=beforeCustom)return 24;states[4].enabled=false;
 customInput=(id)0xa00;customText="nan";customSpeedAction(0,0,0);if(queued!=beforeCustom)return 25;
 customText="2.5";customSpeedAction(0,0,0);if(queued!=beforeCustom+1||jobFeature!=4||!jobEnable||customJobSpeed!=2.5f)return 26;
 busy=0;states[4].enabled=true;customText="invalid";customSpeedAction(0,0,0);if(queued!=beforeCustom+2||jobFeature!=4||jobEnable)return 27;
 busy=0;states[4].enabled=false;menuClosing=false;unsigned beforeRestore=queued;restoreAllAction(0,0,0);if(queued!=beforeRestore)return 28;
 states[1].enabled=true;restoreAllAction(0,0,0);if(jobFeature!=restoreAllJob||jobEnable||queued!=beforeRestore+1)return 29;busy=0;states[1].enabled=false;
 for(long i=0;i<4;i++){senderTag=i;unsigned q=queued;presetAction(0,0,(id)0x200);const char *v[]={"1","2","4","8"};if(!filledText||strcmp(filledText,v[i])||queued!=q)return 35;}
 states[0].enabled=true;senderTag=1;filledText=0;presetAction(0,0,(id)0x200);if(filledText)return 36;states[0].enabled=false;
 bool previousMode=performanceMode;unsigned q=queued;performanceAction(0,0,0);if(performanceMode==previousMode||queued!=q)return 37;performanceAction(0,0,0);if(performanceMode!=previousMode)return 38;
 Rect left=snapEntry({300,100,52,52},{0,0,844,390}),right=snapEntry({600,100,52,52},{0,0,844,390});if(left.x!=8||right.x!=784||left.y!=100||right.y!=100)return 30;
 menuVisible=false;menuClosing=false;idleSerial=100;entryDimmed=false;idleDim((void*)99);if(entryDimmed)return 31;idleDim((void*)100);if(!entryDimmed)return 32;wakeFloating();if(entryDimmed||idleSerial!=101)return 33;
 toggleMenuAction(0,0,0);closeMenu();unsigned oldAnimation=menuAnimationSerial;toggleMenuAction(0,0,0);finishMenuClose((void*)(JJAddr)oldAnimation);if(!menuVisible||popupHidden||menuClosing)return 34;
 printf("PASS: popup/drag/snap/landscape, custom/preset exclusion, restore-all scheduling, performance toggle, animation serials\n");return 0;}
