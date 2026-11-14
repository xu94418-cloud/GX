#define main existing_tests_main
#include "tests.cpp"
#undef main
int main(){
 JJAddr bases[]={0x5000000000ULL,0x5fffffc000ULL,0x9000000000ULL,0xa000000000ULL};
 for(JJAddr base:bases)for(unsigned feature=0;feature<4;feature++){
 Fake f={};f.base=base;JJIO io={&f,reg,read,write,0,0};
 put(f,0x100,jj_float(3.8013333e20f));put(f,0x114,jj_float(1));put(f,0x400,1082977991);put(f,0x800,427281867);put(f,0x810,6);put(f,0x818,jj_float(5));put(f,0x1800,jj_float(.83999997377f));put(f,0x1900,jj_float(.6f));
 JJFeatureState state={};CHECK(!jj_apply_feature(io,feature,true,state));CHECK(f.writes==0);CHECK(state.report.initial==0);
 }
 printf("PASS: every feature excludes addresses below lower bound and at/above upper bound\n");return 0;
}
