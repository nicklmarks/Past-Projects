/*	Gamma - Generic processing library
 See COPYRIGHT file for authors and license information
 
 synth1.cpp
 A very simple synthesis instrument (just a sine wave and an amplitude envelope)
 plus a minute-long notelist-style fixed composition consisting of 64 notes played
 by this instrument.
 
 */

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Scheduler.h"
using namespace gam;
#include "allocore/io/al_App.hpp"


struct SineEnv : public Process<AudioIOData> {
    float mAmp;
    float mDur;
    Pan<> mPan;
    Sine<> mOsc;
    Env<3> mAmpEnv;
    
    void onProcess(AudioIOData& io){
        mAmpEnv.totalLength(mDur, 1);
        
        while(io()) {
            float s1 = mOsc() * mAmpEnv() * mAmp;
            float s2;
            mPan(s1, s1,s2);
            io.out(0) += s1;
            io.out(1) += s2;
        }
        if(mAmpEnv.done()) free();
    }
    
    
    SineEnv& freq(float v){ mOsc.freq(v); return *this; }
    SineEnv& amp(float v){ mAmp=v; return *this; }
    SineEnv& attack(float v) {
        mAmpEnv.lengths()[0] = v;
        return *this;
    }
    SineEnv& decay(float v) {
        mAmpEnv.lengths()[2] = v;
        return *this;
    }
    SineEnv& dur(float v){ mDur=v; return *this; }
    SineEnv& pan(float v){ mPan.pos(v); return *this; }
    SineEnv& set(float a, float b, float c, float d, float e, float f=0) {
        return dur(a).freq(b).amp(c).attack(d).decay(e).pan(f);
    }
    
    SineEnv(double startTime=0) {
        set(6.5, 60, 0.3, 1, 2);
        dt(startTime);
        mAmpEnv.curve(0); // make segments lines
        mAmpEnv.levels(0,1,1,0);
    }
};


struct OscTrm : public Process<AudioIOData> {
	float mAmp;
	float mDur;
	float mTrmDepth;
	Sine<> mTrm;
	Pan<> mPan;
	Env<2> mTrmEnv;
	Osc<> mOsc;
	Env<3> mAmpEnv;
	EnvFollow<> mEnvFollow;
	
	void onProcess(AudioIOData& io){
		
		mAmpEnv.totalLength(mDur, 1);
		mTrmEnv.totalLength(mDur);
		
		while(io()){
			
			mTrm.freq(mTrmEnv());
			//float trmAmp = mAmp - mTrm()*mTrmDepth; // Replaced with line below
			float trmAmp = (mTrm()*0.5+0.5)*mTrmDepth + (1-mTrmDepth); // Corrected
			float s1 = mOsc() * mAmpEnv() * trmAmp * mAmp;
			float s2;
			mEnvFollow(s1);
			mPan(s1, s1,s2);
			io.out(0) += s1;
			io.out(1) += s2;
		}
		if(mAmpEnv.done() && (mEnvFollow.value() < 0.001)) free();
	}
	
	OscTrm& freq(float v){ mOsc.freq(v); return *this; }
	OscTrm& amp(float v){ mAmp=v; return *this; }
	OscTrm& dur(float v){ mDur=v; return *this; }
	OscTrm& attack(float v){ mAmpEnv.lengths()[0]=v; return *this; }
	OscTrm& decay(float v){  mAmpEnv.lengths()[2]=v; return *this; }
	OscTrm& sus(float v){ mAmpEnv.levels()[2]=v; return *this; }
	
	OscTrm& trm1(float v){ mTrmEnv.levels(v, mTrmEnv.levels()[1], v); return *this; }
	OscTrm& trm2(float v){ mTrmEnv.levels()[1]=v; return *this; }
	OscTrm& trmDepth(float v){ mTrmDepth=v; return *this; }
	OscTrm& trmRise(float v){ mTrmEnv.lengths(v,1-v); return *this; }
	
	OscTrm& table(ArrayPow2<float>& v){ mOsc.source(v); return *this; }
	
	OscTrm& pan(float v){ mPan.pos(v); return *this; }
	
	
	OscTrm& set(
				float a, float b, float c, float d, float e, float f,
				float g, float h, float i, float j,
				ArrayPow2<float>& k, float l
				){
		return dur(a).freq(b).amp(c).attack(d).decay(e).sus(f)
		.trmDepth(g).trm1(h).trm2(i).trmRise(j)
		.table(k).pan(l);
	}
	
	OscTrm(double startTime=0)
	:	mAmp(1), mDur(2)
	{
		dt(startTime);
		set(10, 262, 0.5, 0.1,2,0.8, 0.4,4,8,0.5, mOsc, 0.8);
		mAmpEnv.levels(0,1,1,0);
	}
	
};


struct AddSyn : public Process<AudioIOData> {
    
	float mAmp;
	float mAmpStri;
	float mAmpLow;
	float mAmpUp;
	float mDur;
	float mOscFrq;
	float mfreqStri1, mfreqStri2, mfreqStri3, mfreqLow1, mfreqLow2, mfreqUp1, mfreqUp2, mfreqUp3, mfreqUp4;
	Pan<> mPan;
	Sine<> mOsc;
	Sine<> mOsc1;
	Sine<> mOsc2;
	Sine<> mOsc3;
	Sine<> mOsc4;
	Sine<> mOsc5;
	Sine<> mOsc6;
	Sine<> mOsc7;
	Sine<> mOsc8;
	Sine<> mOsc9;
	Env<3> mEnvStri;
	Env<3> mEnvLow;
	Env<3> mEnvUp;
	EnvFollow<> mEnvFollow;
	
	void onProcess(AudioIOData& io){
		
		mEnvStri.totalLength(mDur, 1);
		mEnvLow.totalLength(mDur, 1);
		mEnvUp.totalLength(mDur, 1);
		
		while(io()){
			float s1 = (mOsc1() + mOsc2() + mOsc3()) * mEnvStri() * mAmpStri;
			s1 += (mOsc4() + mOsc5()) * mEnvLow() * mAmpLow;
			s1 += (mOsc6() + mOsc7() + mOsc8() + mOsc9()) * mEnvUp() * mAmpUp;
			s1 *= mAmp;
			float s2;
			mEnvFollow(s1);
			mPan(s1, s1,s2);
			io.out(0) += s1;
			io.out(1) += s2;
		}
		if(mEnvStri.done() && (mEnvFollow.value() < 0.0001)) free();
	}
	
	AddSyn& freq(float v){
		mOscFrq=v;
		
		mOsc1.freq(mfreqStri1*mOscFrq);
		mOsc2.freq(mfreqStri2*mOscFrq);
		mOsc3.freq(mfreqStri3*mOscFrq);
		mOsc4.freq(mfreqLow1*mOscFrq);
		mOsc5.freq(mfreqLow2*mOscFrq);
		mOsc6.freq(mfreqUp1*mOscFrq);
		mOsc7.freq(mfreqUp2*mOscFrq);
		mOsc8.freq(mfreqUp3*mOscFrq);
		mOsc9.freq(mfreqUp4*mOscFrq);
		return *this;
	}
	
	AddSyn& freqStri1(float v){ mfreqStri1=v; mOsc1.freq(v*mOscFrq); return *this; }
	AddSyn& freqStri2(float v){ mfreqStri2=v; mOsc2.freq(v*mOscFrq); return *this; }
	AddSyn& freqStri3(float v){ mfreqStri3=v; mOsc3.freq(v*mOscFrq); return *this; }
	AddSyn& freqLow1(float v){ mfreqLow1=v; mOsc4.freq(v*mOscFrq); return *this; }
	AddSyn& freqLow2(float v){ mfreqLow2=v; mOsc5.freq(v*mOscFrq); return *this; }
	AddSyn& freqUp1(float v){ mfreqUp1=v; mOsc6.freq(v*mOscFrq); return *this; }
	AddSyn& freqUp2(float v){ mfreqUp2=v; mOsc7.freq(v*mOscFrq); return *this; }
	AddSyn& freqUp3(float v){ mfreqUp3=v; mOsc8.freq(v*mOscFrq); return *this; }
	AddSyn& freqUp4(float v){ mfreqUp4=v; mOsc9.freq(v*mOscFrq); return *this; }
	AddSyn& freqUp(float v1, float v2, float v3, float v4) {
		mOsc6.freq(v1*mOscFrq);
		mOsc7.freq(v2*mOscFrq);
		mOsc8.freq(v3*mOscFrq);
		mOsc9.freq(v4*mOscFrq);
		return *this;
	}
	
	AddSyn& freq(float v1, float v2) {
		dur(v1);
		freq(v2);
		return *this;
	}
	
	AddSyn& amp(float v){ mAmp=v; return *this; }
	AddSyn& ampStri(float v){ mAmpStri=v; return *this; }
	AddSyn& attackStri(float v){ mEnvStri.lengths()[0] = v; return *this; }
	AddSyn& decayStri(float v){ mEnvStri.lengths()[2] = v; return *this; }
	AddSyn& susStri(float v){ mEnvStri.levels()[2]=v; return *this; }
	AddSyn& ampLow(float v){ mAmpLow=v; return *this; }
	AddSyn& attackLow(float v){ mEnvLow.lengths()[0] = v; return *this; }
	AddSyn& decayLow(float v){ mEnvLow.lengths()[2] = v; return *this; }
	AddSyn& susLow(float v){ mEnvLow.levels()[2]=v; return *this; }
	AddSyn& ampUp(float v){ mAmpUp=v; return *this; }
	AddSyn& attackUp(float v){ mEnvUp.lengths()[0] = v; return *this; }
	AddSyn& decayUp(float v){ mEnvUp.lengths()[2] = v; return *this; }
	AddSyn& susUp(float v){ mEnvUp.levels()[2]=v; return *this; }
	
	
	AddSyn& dur(float v){ mDur=v; return *this; }
	
	AddSyn& pan(float v){ mPan.pos(v); return *this; }
	
	AddSyn& set(
				float a, float b, float c, float d, float e,float f, float g, float h, float i, float j,
				float k, float l, float m, float n, float o, float p, float q, float r, float s, float t,
				float u, float v, float w, float x, float y=0
				){
		return dur(a).freq(b).amp(c).ampStri(d).attackStri(e).decayStri(f).susStri(g).ampLow(h)
		.attackLow(i).decayLow(j).susLow(k).ampUp(l).attackUp(m).decayUp(n).susUp(o)
		.freqStri1(p).freqStri2(q).freqStri3(r).freqLow1(s).freqLow2(t).freqUp1(u)
		.freqUp2(v).freqUp3(w).freqUp4(x).pan(y);
	}
	
    AddSyn(double startTime=0)
    
	{
		dt(startTime);
		set(6.2,155.6,0.01,0.5,0.1,0.1,0.8,0.5,0.001,0.1,0.8,0.6,0.01,0.075,0.9,1,2.001,3,4.00009,5.0002,6,7,8,9);
		//set(6.2,155.6,0.01,0.5,0.01,0.01,0.8,0.5,0.001,0.1,0.8,0.6,0.01,0.075,0.9,1,2.001,3,4.00009,5.0002,6,7,8,9);
		mEnvStri.curve(-4); // make segments lines
		mEnvStri.levels(0,1,1,0);
		mEnvLow.curve(-4); // make segments lines
		mEnvLow.levels(0,1,1,0);
		mEnvUp.curve(-4); // make segments lines
		mEnvUp.levels(0,1,1,0);
	}
	
};

struct Chimes : public AddSyn {
    
	Chimes(double startTime=0) :AddSyn(startTime) {
		//set (6.2,440,0.1,0.5,0.0001,3.8,0.3,0.4,0.0001,6.0,0.99,0.3,0.0001,6.0,0.9,2,3,4.07,0.56,0.92,1.19,1.7,2.75,3.36);
		set (6.2,440,0.1,0.05,0.0001,3.8,0.3,0.04,0.0001,6.0,0.99,0.03,0.0001,6.0,0.9,2,3,4.07,0.56,0.92,1.19,1.7,2.75,3.36);
	}
};


struct Trumpet : public AddSyn {
    
	Trumpet(double startTime=0) :AddSyn(startTime) {
		//set (6.2,440,0.1,0.5,0.0001,3.8,0.3,0.4,0.0001,6.0,0.99,0.3,0.0001,6.0,0.9,2,3,4.07,0.56,0.92,1.19,1.7,2.75,3.36);
		set (6.2,440,0.5,0.05,0.0001,3.8,0.3,0.04,0.0001,6.0,0.99,0.03,0.0001,6.0,0.9,2,3,4.07,0.56,0.92,1.19,1.7,2.75,3.36);
	}
};


float rand(float a, float b) {
    float x = al::rnd::uniform(a,b) ;
    return x ;
}

int randint(int a, int b) {
    int x = al::rnd::uniform(a,b) ;
//    std::cout << x << std::endl ;
    return x;
}

void sinQ (Scheduler &s, float time, float freq, float len, float amp = 0.3) {
    float atk = len * 0.25;
    float dcy = len * 0.5;
    s.add<SineEnv>( time ).set( len, freq, amp, atk, dcy);
}

void sinWhole (Scheduler &s, float time, float freq, float len, float a, float d, float dt=1) {
    float atk = len * 0.25 + a;
    float dcy = 1 + d;
    s.add<SineEnv>( time ).set( len, freq, 0.1, atk * dt, dcy * dt);
}

float halfStepScale[20];
float halfStepInterval = 1.05946309; // 2^(1/12)
void initScaleTo12TET(float lowest) {
	float f = lowest;
	for (int i=0;i<20;++i) {
		halfStepScale[i] = f;
		f *= halfStepInterval;
	}
}

float randomFrom12TET() {
	int index = rnd::uni(0,20);
	// std::cout << "index " << index << " is " << myScale[index] << std::endl;
	return halfStepScale[index];
}


void fillTime(Scheduler &s, float from, float to, float minattackStri, float minattackLow, float minattackUp, float maxattackStri, float maxattackLow, float maxattackUp, float minFreq, float maxFreq, float a) {
	while (from <= to) {
		float nextAtt = rnd::uni((minattackStri+minattackLow+minattackUp),(maxattackStri+maxattackLow+maxattackUp));
		s.add<Chimes>(from).attackStri(nextAtt).freq(rnd::uni(minFreq,maxFreq)).amp(a);
//		std::cout << "old from " << from << " plus nextnextAtt " << nextAtt << std::endl;
		from += nextAtt;
	}
}

void fillTimeWith12TET(Scheduler &s, float from, float to, float minattackStri, float minattackLow, float minattackUp, float maxattackStri, float maxattackLow, float maxattackUp, float a) {
	while (from <= to) {
		float nextAtt = rnd::uni((minattackStri+minattackLow+minattackUp),(maxattackStri+maxattackLow+maxattackUp));
		float f = randomFrom12TET();
		s.add<Chimes>(from).attackStri(nextAtt).freq(f).amp(a);
//		std::cout << "12 old from " << from << " plus nextAtt " << nextAtt << std::endl;
//		std::cout << "12 old from " << from << " plus nextAtt " << nextAtt << std::endl;
		from += nextAtt;
	}
}



int main() {
    Scheduler s;
    
    ArrayPow2<float>
    tbSaw(2048), tbSqr(2048), tbImp(2048), tbSin(2048), tbPls(2048),
    tb__1(2048), tb__2(2048), tb__3(2048), tb__4(2048);
    
	addSinesPow<1>(tbSaw, 9,1);
	addSinesPow<1>(tbSqr, 9,2);
	addSinesPow<0>(tbImp, 9,1);
	addSine(tbSin);
    
	{	float A[] = {1,1,1,1,0.7,0.5,0.3,0.1};
		addSines(tbPls, A,8);
	}
    
	{	float A[] = {1, 0.4, 0.65, 0.3, 0.18, 0.08};
		float C[] = {1,4,7,11,15,18};
		addSines(tb__1, A,C,6);
	}
    
	// inharmonic partials
	{	float A[] = {0.5,0.8,0.7,1,0.3,0.4,0.2,0.12};
		float C[] = {3,4,7,8,11,12,15,16};
		addSines(tb__2, A,C,7);
	}
    
	// inharmonic partials
	{	float A[] = {1, 0.7, 0.45, 0.3, 0.15, 0.08};
		float C[] = {10, 27, 54, 81, 108, 135};
		addSines(tb__3, A,C,6);
	}
    
	// harmonics 20-27
	{	float A[] = {0.2, 0.4, 0.6, 1, 0.7, 0.5, 0.3, 0.1};
		addSines(tb__4, A,8, 20);
	}
    
    
    //     s.add<OscTrm>( time ).set(len, freq, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);

    float time( 0.0f ), len(1), freq(220), amp(0.1), atk(0.1), dcy(0.1), sus(0.1), one(0.01), two(7), three(5), car(1), mod(1.0007), rate1(3.5), rate2(7.5), rise(0.5), depth(0.0075), pan(0), cv(4) ;
    
    float fundamentalTones[] = { 16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87 } ;
    float rnd(0) , rnd2(0) , diff(0) ;
    int rndint(0), rndint2(0) ;
    float saveTime = 0, time1 = 0;
    float time2 = time ;
    float time3 = time ;

    int tempo = 60;
    float dt = (float)60 / tempo ;
    
    float bassline[] = { 261.63, 207.65, 196.00, 174.61, 164.81, 146.83} ;
    float bassline2[] = { 174.61, 174.61, 196.00, 174.61, 146.83, 123.47} ;
    float fluteMel[] = {0, 1046.50, 1174.66, 1318.51, 880.00, 830.61, 880.00, 987.77, 739.99, 659.25, 493.88, 523.25 } ;
    float fluteMel2[] = {0 , 523.25 , 587.33 , 659.255 , 440 , 415.305 , 440 , 493.885 , 369.995 , 329.625 , 246.94 , 261.625 } ;
    float fluteLen[] = {1, 2, 0.5, 0.5, 4, 3, 0.5, 0.5, 1, 1, 1, 1} ;


//    float melody[] = { 987.77, 1046.50, 1108.73, 783.99, 698.46, 659.25, 523.25, 587.33 } ;
    
    // A section
    //chord
    sinWhole (s, time, 600, dt * 5, -0.06, 0.0, dt) ;
    sinWhole (s, time, 650, dt * 5, -0.03, 0.0, dt) ;
    sinWhole (s, time, 720, dt * 5, -0.01, -0.01, dt) ;
    sinWhole (s, time, 780, dt * 5, 0.03, -0.03, dt) ;
    sinWhole (s, time, 860, dt * 5, 0.05, -0.07, dt) ;

    time += dt * 4 ;
    
    // bass Chord
    sinWhole (s, time, 55 * 0.25, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 0.5, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 110, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 2/3, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 4/5 , dt * 5, 0, 0, dt) ;
    
    //set tempo
    tempo = tempo * 2;
    dt = (float)60 / tempo ;
    saveTime = time;
    time += dt * 0.5;
    
    //A section melody
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }

    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }

    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }
    time += -dt * 0.5 ;
    
    
    sinWhole (s, time, 600 * 0.8, dt * 8, -1.9, 3.5, dt) ;
    sinWhole (s, time, 650 * 0.8, dt * 8, -1.6, 3, dt) ;
    sinWhole (s, time, 720 * 0.8, dt * 8, -1.4, 2.6, dt) ;
    sinWhole (s, time, 780 * 0.8, dt * 8, -1, 2.2, dt) ;
    sinWhole (s, time, 860 * 0.8, dt * 8, -0.5, 1.6, dt) ;

    sinWhole (s, time, 600 * 0.4, dt * 8, -1.9, -0.95, dt) ;
    sinWhole (s, time, 650 * 0.4, dt * 8, 0.4, -0.95, dt) ;
    sinWhole (s, time, 720 * 0.4, dt * 8, 1.2, -0.95, dt) ;
    sinWhole (s, time, 780 * 0.4, dt * 8, 2, -0.95, dt) ;
    sinWhole (s, time, 860 * 0.4, dt * 8, 2.7, -0.95, dt) ;

    sinWhole (s, time, 70, dt * 8, -1.5, -0.9, dt) ;
    sinWhole (s, time, 93, dt * 8, -1.3, -0.8, dt) ;
    sinWhole (s, time, 113, dt * 8, -1.0, -0.5, dt) ;
    sinWhole (s, time, 136, dt * 8, -0.8, -0.7, dt) ;
    sinWhole (s, time, 168, dt * 8, -0.5, -0.3, dt) ;
    // END A section
    
    // *******************************************************************************
    
    // Possibly insert AddSyn for this gap...
    
//    initScaleTo12TET(420);
//    amp = 0.01;
//    fillTime(s,time,time + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, 0.05);
//    
//    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
//    time += 2; amp = 0.05;
//    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
//    
//    time += 2;
//    
//    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
//    time += 2; amp = 0.07;
//    
//    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
//    
//    time += 2; amp = 0.1;
//    
//    
//    fillTime(s,time,time + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, 0.05);
//    
    // *******************************************************************************
    // Repeat A section twice as Fast.
    time +=3 ;
    tempo = 120 ;
    dt = (float)60 / tempo ;
    freq = 220 ;
    
    sinWhole (s, time, 600, dt * 5, -0.06, 0.0, dt) ;
    sinWhole (s, time, 650, dt * 5, -0.03, 0.0, dt) ;
    sinWhole (s, time, 720, dt * 5, -0.01, -0.01, dt) ;
    sinWhole (s, time, 780, dt * 5, 0.03, -0.03, dt) ;
    sinWhole (s, time, 860, dt * 5, 0.05, -0.07, dt) ;
    
    time += dt * 4 ;
    
    sinWhole (s, time, 55 * 0.25, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 0.5, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 110, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 2/3, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 4/5 , dt * 5, 0, 0, dt) ;
    
    tempo = tempo * 2;
    dt = (float)60 / tempo ;
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }

    time += -dt * 0.5 ;
    
    sinWhole (s, time, 600 * 0.8, dt * 8, -1.9, 3.5, dt) ;
    sinWhole (s, time, 650 * 0.8, dt * 8, -1.6, 3  , dt) ;
    sinWhole (s, time, 720 * 0.8, dt * 8, -1.4, 2.6, dt) ;
    sinWhole (s, time, 780 * 0.8, dt * 8, -1  , 2.2, dt) ;
    sinWhole (s, time, 860 * 0.8, dt * 8, -0.5, 1.6, dt) ;
    
    sinWhole (s, time, 600 * 0.4, dt * 8, -1.9, -0.95, dt) ;
    sinWhole (s, time, 650 * 0.4, dt * 8, 0.4 , -0.95, dt) ;
    sinWhole (s, time, 720 * 0.4, dt * 8, 1.2 , -0.95, dt) ;
    sinWhole (s, time, 780 * 0.4, dt * 8, 2   , -0.95, dt) ;
    sinWhole (s, time, 860 * 0.4, dt * 8, 2.7 , -0.95, dt) ;
    
    
    sinWhole (s, time, 70,  dt * 8, -1.5, -0.9, dt) ;
    sinWhole (s, time, 93,  dt * 8, -1.3, -0.8, dt) ;
    sinWhole (s, time, 113, dt * 8, -1.0, -0.5, dt) ;
    sinWhole (s, time, 136, dt * 8, -0.8, -0.7, dt) ;
    sinWhole (s, time, 168, dt * 8, -0.5, -0.3, dt) ;
    // END A section at 120
    
    // *******************************************************************************
    
    // Possibly insert AddSyn for this gap...
    
    // *******************************************************************************
    // Repeat A section at 180.

    time +=3 ;
    tempo = 180 ;
    dt = (float)60 / tempo ;
    freq = 220 ;

    sinWhole (s, time, 600, dt * 5, -0.06, 0.0, dt) ;
    sinWhole (s, time, 650, dt * 5, -0.03, 0.0, dt) ;
    sinWhole (s, time, 720, dt * 5, -0.01, -0.01, dt) ;
    sinWhole (s, time, 780, dt * 5, 0.03, -0.03, dt) ;
    sinWhole (s, time, 860, dt * 5, 0.05, -0.07, dt) ;
    
    time += dt * 4 ;
    
    sinWhole (s, time, 55 * 0.25, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 0.5, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 110, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 2/3, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 4/5 , dt * 5, 0, 0, dt) ;
    
    
    tempo = tempo * 2 ;
    dt = (float)60 / tempo ;
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }

    time += -dt * 0.5 ;
    
    sinWhole (s, time, 600 * 0.8, dt * 8, -1.9, 3.5, dt) ;
    sinWhole (s, time, 650 * 0.8, dt * 8, -1.6, 3, dt) ;
    sinWhole (s, time, 720 * 0.8, dt * 8, -1.4, 2.6, dt) ;
    sinWhole (s, time, 780 * 0.8, dt * 8, -1, 2.2, dt) ;
    sinWhole (s, time, 860 * 0.8, dt * 8, -0.5, 1.6, dt) ;
    
    sinWhole (s, time, 600 * 0.4, dt * 8, -1.9, -0.95, dt) ;
    sinWhole (s, time, 650 * 0.4, dt * 8, 0.4, -0.95, dt) ;
    sinWhole (s, time, 720 * 0.4, dt * 8, 1.2, -0.95, dt) ;
    sinWhole (s, time, 780 * 0.4, dt * 8, 2, -0.95, dt) ;
    sinWhole (s, time, 860 * 0.4, dt * 8, 2.7, -0.95, dt) ;
    
    sinWhole (s, time, 70, dt * 8, -1.5, -0.9, dt) ;
    sinWhole (s, time, 93, dt * 8, -1.3, -0.8, dt) ;
    sinWhole (s, time, 113, dt * 8, -1.0, -0.5, dt) ;
    sinWhole (s, time, 136, dt * 8, -0.8, -0.7, dt) ;
    sinWhole (s, time, 168, dt * 8, -0.5, -0.3, dt) ;
    // END A section at 180
    
    // *******************************************************************************
    
    // Possibly insert AddSyn for this gap...
    
    // *******************************************************************************
    // Repeat A section at 270.

    time +=3 ;
    tempo = 270 ;
    dt = (float)60 / tempo ;
    
    freq = 220 ;
    
    sinWhole (s, time, 600, dt * 5, -0.06, 0.0, dt) ;
    sinWhole (s, time, 650, dt * 5, -0.03, 0.0, dt) ;
    sinWhole (s, time, 720, dt * 5, -0.01, -0.01, dt) ;
    sinWhole (s, time, 780, dt * 5, 0.03, -0.03, dt) ;
    sinWhole (s, time, 860, dt * 5, 0.05, -0.07, dt) ;
    
    time += dt * 4 ;
    
    sinWhole (s, time, 55 * 0.25, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 0.5, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 110, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 2/3, dt * 5, 0, 0, dt) ;
    sinWhole (s, time, 55 * 4/5 , dt * 5, 0, 0, dt) ;
    
    tempo = tempo * 2 ;
    dt = (float)60 / tempo ;
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }

    time += -dt * 0.5 ;
    
    sinWhole (s, time, 600 * 0.8, dt * 8, -1.9, 3.5, dt) ;
    sinWhole (s, time, 650 * 0.8, dt * 8, -1.6, 3, dt) ;
    sinWhole (s, time, 720 * 0.8, dt * 8, -1.4, 2.6, dt) ;
    sinWhole (s, time, 780 * 0.8, dt * 8, -1, 2.2, dt) ;
    sinWhole (s, time, 860 * 0.8, dt * 8, -0.5, 1.6, dt) ;
    
    sinWhole (s, time, 600 * 0.4, dt * 8, -1.9, -0.95, dt) ;
    sinWhole (s, time, 650 * 0.4, dt * 8, 0.4, -0.95, dt) ;
    sinWhole (s, time, 720 * 0.4, dt * 8, 1.2, -0.95, dt) ;
    sinWhole (s, time, 780 * 0.4, dt * 8, 2, -0.95, dt) ;
    sinWhole (s, time, 860 * 0.4, dt * 8, 2.7, -0.95, dt) ;
    
    sinWhole (s, time, 70, dt * 8, -1.5, -0.9, dt) ;
    sinWhole (s, time, 93, dt * 8, -1.3, -0.8, dt) ;
    sinWhole (s, time, 113, dt * 8, -1.0, -0.5, dt) ;
    sinWhole (s, time, 136, dt * 8, -0.8, -0.7, dt) ;
    sinWhole (s, time, 168, dt * 8, -0.5, -0.3, dt) ;
    
//    time += 4;
//    std::cout << time << std::endl;
    
    // start another time here that is more of a melody, use the tremolo instrument to get space alien oscilatey sounds
    
    time2 = time ;
    
    initScaleTo12TET(420);
    amp = 0.01;
    fillTime(s,time,time + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, 0.05);

    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time += 2; amp = 0.05;
    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    
    time += 2;

    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time += 2; amp = 0.07;

    fillTime(s,time,time + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);

    time += 2; amp = 0.1;

    
    fillTime(s,time,time + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, 0.05);

    
    // ************************************************************************

    
    initScaleTo12TET(640);
    amp = 0.05;
    fillTime(s,time2,time2 + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, amp);
    
    fillTime(s,time2,time2 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time2 += 2; amp = 0.08;
    fillTime(s,time2,time2 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    
    time2 += 2;
    
    fillTime(s,time2,time2 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time2 += 2; amp = 0.07;
    
    fillTime(s,time2,time2 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    
    time2 += 2; amp = 0.05;
    
    
    fillTime(s,time2,time2 + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, amp);
    
    // ************************************************************************
    
    
    initScaleTo12TET(640);
    amp = 0.05;
    fillTime(s,time3,time3 + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, amp);
    
    fillTime(s,time3,time3 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time3 += 2; amp = 0.08;
    fillTime(s,time3,time3 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    
    time3 += 2;
    
    fillTime(s,time3,time3 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    time3 += 2; amp = 0.07;
    
    fillTime(s,time3,time3 + 2, rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.00005, 0.006), rand(0.05, 0.6), rand(0.05, 0.6), rand(0.05, 0.6), rand(200, 500), rand(500, 800), amp);
    
    time3 += 2; amp = 0.05;
    
    
    fillTime(s,time3,time3 + 10, 0.0001, 0.0001, 0.0001, 0.1, 0.1, 0.1, 200, 500, amp);
    
    
    // ************************************************************************
    // Depressing interlude melody
    time +=2;
    
    amp = 0.1 ; atk = 0.1 ; dcy = 0.1 ; sus = 0.1 ;
    tempo = 90;
    dt = (float)60 / tempo ;
    
    
    time1 = time ;
    
    for (int i =0; i<12; i++) {
        len = dt * fluteLen[i];
        if ( i == 5)
            s.add<OscTrm>( time1 ).set(dt * 4, fluteMel[5], amp, len * 0.1 , len * 0.2 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        if ( i == 8)
            s.add<OscTrm>( time1 ).set(dt * 4, fluteMel[8], amp, len * 0.1 , len * 0.15 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        if (i == 6) { }
        else if (i == 7) { }
        else if (i > 8) { }
        else {
            s.add<OscTrm>( time1 ).set(len, fluteMel[i], amp, len * 0.1 , len * 0.1 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        }
        
        time1 += len;
    }
    
    tempo = 80;
    dt = (float)60 / tempo ;

    for (int i =0; i<12; i++) {
        len = dt * fluteLen[i];
        s.add<OscTrm>( time1 ).set(len, fluteMel[i], amp, len * 0.12 , len * 0.15 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        s.add<OscTrm>( time1 ).set(len, fluteMel2[i], amp, len * 0.12 , len * 0.15 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        time1 += len;
    }
    
    tempo = 60;
    dt = (float)60 / tempo ;

    float temptime = time1;
    for (int i=0; i<15; i++) {
        freq = fluteMel[i%6] + 1 ; len = 0.5 ;
        s.add<SineEnv>( temptime ).set( len, freq, 0.1, 0.03, 0.03);
        temptime += len;
    }
    for (int i=0; i<15; i++) {
        freq = fluteMel[randint(1,7)] + 1 ; len = 0.5 ;
        s.add<SineEnv>( temptime ).set( len, freq, 0.15, 0.03, 0.03);
        s.add<SineEnv>( temptime ).set( len, fluteMel[i%6] + 1, 0.15, 0.03, 0.03);
        temptime += len;
    }
    temptime += 0.25 ;
    for (int i=0; i<7; i++) {
        freq = fluteMel[i%6] + 1 ; len = 0.5 ;
        s.add<SineEnv>( temptime ).set( len, freq, rand(0.15, 0.22), 0.03, 0.03);
        temptime += len;
    }
    
    for (int i =0; i<12; i++) {
        len = dt * fluteLen[i];
        s.add<OscTrm>( time1 ).set(len, fluteMel[i], amp, len * 0.1 - 0.05, len * 0.3 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        s.add<OscTrm>( time1 ).set(len, fluteMel2[i], amp, len * 0.1 - 0.05, len * 0.3 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
        time1 += len;
    }
    
    for (int i=0; i<15; i++) {
        freq = fluteMel[randint(1,7)] + 1 ; len = rand(0.3, 1.4) ;
        sinQ (s, time1, freq, len, rand(0.15, 0.22)) ;
        time1 += len;
    }
    
    
    // START BASS
    tempo = 90;
    dt = (float)60 / tempo ;

    for (int i = 0; i<4; i++) {
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], 0.3, dt * 0.05, dt * 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], 0.3, dt * 3   , dt * 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        time += dt * 4;
    }
    
    tempo = 80;
    dt = (float)60 / tempo ;

    for (int i = 0; i<4; i++) {
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], amp, dt * 0.05, dt * 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], amp, dt * 3   , dt * 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4] * 0.5, amp, dt * 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4] * 0.5, amp, dt * 3   , dt * 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        time += dt * 4;
    }
    
    tempo = 60;
    dt = (float)60 / tempo ;

    int note = 0;
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    time += dt * 4;
    note++;
    
    sus = 0.05 ;
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    time += dt * 4;
    note++;
    
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
    time += dt * 4;
    note++;
    
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
    time += dt * 4;
    
    note = 0;
    
    saveTime = time ;
    
    for (int i = 0; i<1; i++) {
        note = 0;
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        time += dt * 4;
        note++;
        
        sus = 0.05 ;
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        time += dt * 4;
        note++;
        
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
        time += dt * 4;
        note++;
        
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
        time += dt * 4;
        
        note = 0;
        
    }
    // END BASS
    
    
    
    //FIRST HALF
    //**************************************************************
    
    
    len = 4 ; freq = freq * (5/3) ; amp = 0.3 ; atk = 0.2; dcy = 1; sus = 0.1; depth = 0.4; one = 80; two = 80000; rise = 0.5; pan = 0;
    s.add<OscTrm>( time ).set(len, freq, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 3.4 ).set(len, freq + diff * 15, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 4.3 ).set(len, freq + diff * 9, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    
    len = 7 ; freq = 512 ; amp = 0.3 ; atk = 0.2; dcy = 1; sus = 0.1; depth = 0.4; one = 80; two = 80000; rise = 0.0; pan = 0;
    s.add<OscTrm>( time + 8 ).set(len, freq, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 10.4 ).set(len, freq * (1/2), amp, atk, dcy, sus, depth, one, two, 1, tbSin, 0.6);
    s.add<OscTrm>( time + 14.6 ).set(len, freq * (3/2), amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    
    time += 15 ;
    
    float bSectionStartTime = time;
    //    float dt3, dt4, dt5, dt6 ;
    time3 = time ;
    time2 = time ;
    float time4 = time ;
    float time5 = time ;
    
    tempo = 90;
    dt = (float)60 / tempo ;
    
    float dtArr[] = {1.5, 1.25, 1.2, 1.332, 1, 2, 0.5, 0.5, 4, 3, 0.5} ;
    float numi[] = {   2,    2,   1,     1, 3, 2,   5,   5, 1, 2,   3} ;
    float ratios[] = {  0.333, 0.667, 1.3334, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 2 , 0.36, 0.4, 0.6667, .69, 0.8, 1.2, 3, 4, 6  };
    float ratiobase3[] = {0.333, 0.6667, 1.3334} ;
    float ratiobase2[] = { 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 } ;
    freq = 420;
    
    tempo = 163;
    dt = (float)60 / tempo ;
    
    float dt3 = dt * 0.3333333333333 ;
    float dt4 = dt * 0.25 ;
    float saveTime2 = time ;
    
    
    time1 = time;
    time3 = time;
    
    // possibly decrescendo on repetitions.
    for (int i = 0; i < 6; i++) {
        for (int j=0; j<4; j++) {
            sinQ (s, time1, 210, dt3 ) ;
            time1 += dt3 ;
            sinQ (s, time1, 210, dt3 , 0.1) ;
            time1 += dt3 ;
            sinQ (s, time1, 210, dt3 , 0.1) ;
            time1 += dt3 ;
        }
    }
    
    time3 += dt3 * 6;
    time2 = time3 + dt4 * 4 ;
    
    for (int i = 0; i < 5; i++) {
        for (int j=0; j<4; j++) {
            sinQ (s, time3, 210 * 1.25 , dt4 * 3 ) ;
            time3 += dt4 * 3 ;
            sinQ (s, time3, 210 , dt4 , 0.1) ;
            time3 += dt4 ;
        }
    }
    
    tempo = 84;
    dt = (float)60 / tempo ;
    dt3 = dt * 0.3333333333333 ;
    
    float df[] = {1.5, 1.25, 1} ;
    
    for (int i = 0; i < 9; i++) {
        tempo += -3 ;
        dt = (float)60 / tempo ;
        dt3 = dt * 0.3333333333333 ;
        
        if (i == 2) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
        else if (i == 5) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
        else if (i == 8) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
        else {
            df[0] = 1.5; df[1] = 1.25; df[2] = 1;
        }
        //        for (int j=0; j<4; j++) {
        sinQ (s, time2, 210 * df[0] * 2, dt3 , 0.3) ;
        time2 += dt3 ;
        sinQ (s, time2, 210 * df[1] * 2, dt3 , 0.15) ;
        time2 += dt3 ;
        sinQ (s, time2, 210 * df[2] * 2, dt3 , 0.07) ;
        time2 += dt3 ;
        //        }
    }
    
    time2 += -dt3;
    
    sinQ (s, time2, 210, dt3 * 5 ) ;
    sinQ (s, time2, 210 * 1.25 , dt3 * 5 ) ;
    
    
    tempo = 59;
    dt = (float)60 / tempo ;
    float dt5 = dt * 0.2 ;
    
    saveTime2 = time2;
    
    time = time2;
    
    len = 4 ; freq = freq * (5/3) ; amp = 0.3 ; atk = 0.2; dcy = 1; sus = 0.1; depth = 0.4; one = 80; two = 80000; rise = 0.5; pan = 0;
    s.add<OscTrm>( time ).set(len, freq, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 3.4 ).set(len, freq + diff * 15, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 4.3 ).set(len, freq + diff * 9, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    
    len = 7 ; freq = 620 ; amp = 0.3 ; atk = 0.2; dcy = 1; sus = 0.1; depth = 0.4; one = 80; two = 80000; rise = 0.0; pan = 0;
    s.add<OscTrm>( time + 8 ).set(len, freq, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    s.add<OscTrm>( time + 10.4 ).set(len, freq * 0.667, amp, atk, dcy, sus, depth, one, two, 1, tbSin, 0.6);
    s.add<OscTrm>( time + 14.6 ).set(len, freq * 0.75, amp, atk, dcy, sus, depth, one, two, rise, tbSin, pan);
    
    
    for (int i=0; i<10; i++) {
        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;  amp = 0.1;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
    }
    
    time2 = saveTime2 + dt5 * 6;
    freq = freq * 2 * .66667 ;
    for (int i=0; i<10; i++) {
        float a = i * 0.01 ;
        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1 - (a);
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;  amp = 0.05 - (a);
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
    }
    
    time2 = saveTime2 + dt5 * 16;
    saveTime2 = time2;
    freq = freq * 2 * .66667 ;
    for (int i=0; i<10; i++) {
        float a = i * 0.01 ;
        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1 - (a);
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;  amp = 0.05 - (a);
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
        time2 += dt5 ;
    }
    
    time1 = time2 ;
    for (int i = 0; i < 6; i++) {
        for (int j=0; j<4; j++) {
            sinQ (s, time1, 210 * 2 , dt3 , 0.05) ;
            time1 += dt3 ;
            sinQ (s, time1, 210 * 2 , dt3 , 0.2) ;
            time1 += dt3 ;
            sinQ (s, time1, 210 * 2 , dt3 , 0.1) ;
            time1 += dt3 ;
        }
    }
    
    for (int i = 0; i < 6; i++) {
        for (int j=0; j<4; j++) {
            sinQ (s, time2, 210 * 1.667 , dt3 * 2, 0.07 ) ;
            time2 += dt3 * 2;
            sinQ (s, time2, 210 * 2 , dt3 * 2, 0.12) ;
            time2 += dt3 * 2;
            sinQ (s, time2, 210 * 1.75 , dt3 * 2, 0.06) ;
            time2 += dt3 * 2;
        }
    }
    
    
    // Repeat A section at 270.
    
    time = saveTime2  ;
    tempo = 270 ;
    dt = (float)60 / tempo ;
    
    freq = 220 ;
    
//    sinWhole (s, time, 600, dt * 5, -0.06, 0.0, dt) ;
//    sinWhole (s, time, 650, dt * 5, -0.03, 0.0, dt) ;
//    sinWhole (s, time, 720, dt * 5, -0.01, -0.01, dt) ;
//    sinWhole (s, time, 780, dt * 5, 0.03, -0.03, dt) ;
//    sinWhole (s, time, 860, dt * 5, 0.05, -0.07, dt) ;
//    
//    time += dt * 4 ;
//    
//    sinWhole (s, time, 55 * 0.25, dt * 5, 0, 0, dt) ;
//    sinWhole (s, time, 55 * 0.5, dt * 5, 0, 0, dt) ;
//    sinWhole (s, time, 55, dt * 5, 0, 0, dt) ;
//    sinWhole (s, time, 110, dt * 5, 0, 0, dt) ;
//    sinWhole (s, time, 55 * 2/3, dt * 5, 0, 0, dt) ;
//    sinWhole (s, time, 55 * 4/5 , dt * 5, 0, 0, dt) ;
    
    tempo = tempo * 2 ;
    dt = (float)60 / tempo ;
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }
    
    time += -dt * 0.5 ;
    
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }
    
    time += -dt * 0.5 ;
    
    saveTime = time;
    saveTime2 = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }
    
    time += -dt * 0.5 ;
    
    saveTime = time;
    time += dt * 0.5;
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (5 * 0.25) ; }
    
    freq = 110 * 9/4;
    time = saveTime + dt * 6.75 ;
    for (int i=0; i<=3; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq * (2 * 0.667) ; }
    
    for (int i=0; i<=5; i++) { sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ; }
    
    time += -dt * 0.5 ;
    
    
//            s.add<OscTrm>( time1 ).set(len, fluteMel[i], amp, len * 0.1 - 0.05, len * 0.3 , sus, 0.4,4,8,0.5, tbSqr, 0.8);

    
//    for (int i=0; i<4; i++) {
//        sinQ (s, time, freq, dt) ; time += dt; freq = freq *  0.667 ;
//    }

    
    
    //************************************************************
    
//    sinWhole (s, time, 600 * 0.8, dt * 8, -1.9, 3.5, dt) ;
//    sinWhole (s, time, 650 * 0.8, dt * 8, -1.6, 3, dt) ;
//    sinWhole (s, time, 720 * 0.8, dt * 8, -1.4, 2.6, dt) ;
//    sinWhole (s, time, 780 * 0.8, dt * 8, -1, 2.2, dt) ;
//    sinWhole (s, time, 860 * 0.8, dt * 8, -0.5, 1.6, dt) ;
//    
//    sinWhole (s, time, 600 * 0.4, dt * 8, -1.9, -0.95, dt) ;
//    sinWhole (s, time, 650 * 0.4, dt * 8, 0.4, -0.95, dt) ;
//    sinWhole (s, time, 720 * 0.4, dt * 8, 1.2, -0.95, dt) ;
//    sinWhole (s, time, 780 * 0.4, dt * 8, 2, -0.95, dt) ;
//    sinWhole (s, time, 860 * 0.4, dt * 8, 2.7, -0.95, dt) ;
//    
//    sinWhole (s, time, 70, dt * 8, -1.5, -0.9, dt) ;
//    sinWhole (s, time, 93, dt * 8, -1.3, -0.8, dt) ;
//    sinWhole (s, time, 113, dt * 8, -1.0, -0.5, dt) ;
//    sinWhole (s, time, 136, dt * 8, -0.8, -0.7, dt) ;
//    sinWhole (s, time, 168, dt * 8, -0.5, -0.3, dt) ;
    
    
    /*
     use these two patterns to go between 3 and 4
     ... ... .||. .||. ... ... .||. .||. ... ... .||. .||.
     
     .|. |.| .|. |.| .|. |.|
     
     for (int i = 0; i < 6; i++) {
     for (int j=0; j<4; j++) {
     sinQ (s, time1, 210, dt3 ) ;
     time1 += dt3 ;
     sinQ (s, time1, 210, dt3 , 0.1) ;
     time1 += dt3 ;
     sinQ (s, time1, 210, dt3 , 0.1) ;
     time1 += dt3 ;
     }
     }
     
     time3 += dt3 * 6;
     time2 = time3 + dt4 * 4 ;
     
     for (int i = 0; i < 5; i++) {
     for (int j=0; j<4; j++) {
     sinQ (s, time3, 210 * 1.25 , dt4 * 3 ) ;
     time3 += dt4 * 3 ;
     sinQ (s, time3, 210 , dt4 , 0.1) ;
     time3 += dt4 ;
     }
     }
     
     
     */
    
    
    
    
    
    
    
    
    
    
    //OLD ********************************
//    float bSectionStartTime = time;
////    float dt3, dt4, dt5, dt6 ;
//    time3 = time ;
//    time2 = time ;
//    float time4 = time ;
//    float time5 = time ;
//    
//    tempo = 90;
//    dt = (float)60 / tempo ;
//    
//    float dtArr[] = {1.5, 1.25, 1.2, 1.332, 1, 2, 0.5, 0.5, 4, 3, 0.5} ;
//    float numi[] = {   2,    2,   1,     1, 3, 2,   5,   5, 1, 2,   3} ;
//    float ratios[] = {  0.333, 0.667, 1.3334, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 2 , 0.36, 0.4, 0.6667, .69, 0.8, 1.2, 3, 4, 6  };
//    float ratiobase3[] = {0.333, 0.6667, 1.3334} ;
//    float ratiobase2[] = { 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 } ;
//    freq = 420;
//    
//    tempo = 163;
//    dt = (float)60 / tempo ;
//
//    float dt3 = dt * 0.3333333333333 ;
//    float dt4 = dt * 0.25 ;
//    float saveTime2 = time ;
//
//    
//    time1 = time;
//    time3 = time;
//    
//    // possibly decrescendo on repetitions.
//    for (int i = 0; i < 6; i++) {
//        for (int j=0; j<4; j++) {
//            sinQ (s, time1, 210, dt3 ) ;
//            time1 += dt3 ;
//            sinQ (s, time1, 210, dt3 , 0.1) ;
//            time1 += dt3 ;
//            sinQ (s, time1, 210, dt3 , 0.1) ;
//            time1 += dt3 ;
//        }
//    }
//
//    time3 += dt3 * 6;
//    time2 = time3 + dt4 * 4 ;
//    
//    for (int i = 0; i < 5; i++) {
//        for (int j=0; j<4; j++) {
//            sinQ (s, time3, 210 * 1.25 , dt4 * 3 ) ;
//            time3 += dt4 * 3 ;
//            sinQ (s, time3, 210 , dt4 , 0.1) ;
//            time3 += dt4 ;
//        }
//    }
//    
//    tempo = 84;
//    dt = (float)60 / tempo ;
//    dt3 = dt * 0.3333333333333 ;
//
//    float df[] = {1.5, 1.25, 1} ;
//    
//    for (int i = 0; i < 9; i++) {
//        tempo += -3 ;
//        dt = (float)60 / tempo ;
//        dt3 = dt * 0.3333333333333 ;
//
//        if (i == 2) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
//        else if (i == 5) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
//        else if (i == 8) { df[0] = 1.3334; df[1] = 1.083335; df[2] = 0.87; }
//        else {
//            df[0] = 1.5; df[1] = 1.25; df[2] = 1;
//        }
//        //        for (int j=0; j<4; j++) {
//        sinQ (s, time2, 210 * df[0] * 2, dt3 , 0.3) ;
//        time2 += dt3 ;
//        sinQ (s, time2, 210 * df[1] * 2, dt3 , 0.15) ;
//        time2 += dt3 ;
//        sinQ (s, time2, 210 * df[2] * 2, dt3 , 0.07) ;
//        time2 += dt3 ;
//        //        }
//    }
//    
//    time2 += -dt3;
//    
//    sinQ (s, time2, 210, dt3 * 5 ) ;
//    sinQ (s, time2, 210 * 1.25 , dt3 * 5 ) ;
//    
//    
//    tempo = 59;
//    dt = (float)60 / tempo ;
//    float dt5 = dt * 0.2 ;
//
//    saveTime2 = time2;
//    
//    
//    for (int i=0; i<10; i++) {
//        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;  amp = 0.1;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//    }
//    
//    time2 = saveTime2 + dt5 * 6;
//    freq = freq * 2 * .66667 ;
//    for (int i=0; i<10; i++) {
//        float a = i * 0.01 ;
//        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1 - (a);
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;  amp = 0.05 - (a);
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//    }
//
//    time2 = saveTime2 + dt5 * 16;
//    freq = freq * 2 * .66667 ;
//    for (int i=0; i<10; i++) {
//        float a = i * 0.01 ;
//        len = rand(1.5, 2.4); atk = 0.5; dcy = 0.98; amp = 0.1 - (a);
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;  amp = 0.05 - (a);
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//        s.add<SineEnv>( time2 ).set( len, freq, amp, atk, dcy);
//        time2 += dt5 ;
//    }
//
//    
//    
//    
//    /*
//     use these two patterns to go between 3 and 4
//     ... ... .||. .||. ... ... .||. .||. ... ... .||. .||.
//     
//     .|. |.| .|. |.| .|. |.|
//     
//     for (int i = 0; i < 6; i++) {
//     for (int j=0; j<4; j++) {
//     sinQ (s, time1, 210, dt3 ) ;
//     time1 += dt3 ;
//     sinQ (s, time1, 210, dt3 , 0.1) ;
//     time1 += dt3 ;
//     sinQ (s, time1, 210, dt3 , 0.1) ;
//     time1 += dt3 ;
//     }
//     }
//     
//     time3 += dt3 * 6;
//     time2 = time3 + dt4 * 4 ;
//     
//     for (int i = 0; i < 5; i++) {
//     for (int j=0; j<4; j++) {
//     sinQ (s, time3, 210 * 1.25 , dt4 * 3 ) ;
//     time3 += dt4 * 3 ;
//     sinQ (s, time3, 210 , dt4 , 0.1) ;
//     time3 += dt4 ;
//     }
//     }
//
//     
//     */
    
    
    
    
    
//    s.add<SineEnv>( time ).set( len, freq, amp, atk, dcy);
//    s.add<SineEnv>( time ).set( len, freq, amp, atk, dcy);

    
    
    
    
    
    
    
//    for (int i=0; i < 15; i++) {
////        ratiobase3[]
////        ratiobase2[]
//        float drat[] = { ratiobase3[randint(0,2)],ratiobase3[randint(0,2)] * ratiobase2[randint(0,7)], ratiobase2[randint(0,7)], ratiobase3[randint(0,2)] * ratiobase2[randint(0,7)] * ratiobase2[randint(0,7)]  } ;
//        sinQ (s, time2, 210 * drat[0] , dt3 * 5 ) ;
//        time2 += dt3 * 5 ;
//        if (i > 3) {
//            sinQ (s, time2, 210 * drat[1] , dt3 * 5 ) ;
//        }
////        sinQ (s, time2, 210 * drat[1] , dt3 * 5 ) ;
////        sinQ (s, time2, 210 * drat[2] , dt3 * 5 ) ;
////        sinQ (s, time2, 210 * drat[3] , dt3 * 5 ) ;
//
//    }
    


    
    
    
//    for (int i = 0; i < 8; i++) {
//        for (int j=0; j<2; j++) {
//            sinQ (s, time1, 210, dt3 * 2 ) ;
//            time1 += dt3 * 2;
//            sinQ (s, time1, 210, dt3 * 2, 0.1) ;
//            time1 += dt3 * 2;
//            sinQ (s, time1, 210, dt3 * 2, 0.1) ;
//            time1 += dt3 * 2;
//        }
//    }


    
    tempo = 90;
    dt = (float)60 / tempo ;

    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time3, freq * ratiobase3[randint(0,2)], dt * ratiobase2[randint(3,5)]) ;
//            time3 += dt * ratios[j] ;
//        }
//    }
    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time3, freq * ratiobase3[randint(0,2)], dt * ratiobase2[randint(2,4)]) ;
//            time3 += dt * ratios[j] ;
//        }
//    }
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time3, freq * ratiobase3[randint(0,2)], dt * ratiobase2[randint(1,3)]) ;
//            time3 += dt * ratios[j] ;
//        }
//    }
    
    
    
    
    
    //testing kinda a melody
//    
//    for (int i=0; i<=3; i++) {
//        sinQ (s, time, freq, dt) ;
//        time += dt;
//    }
//
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
////            std::cout << dtArr[j] << " , " ;
//            sinQ (s, time, freq * dtArr[j], dt * dtArr[j]) ;
//            time += dt * dtArr[j] ;
//        }
//    }
//
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(0,3)], dt * ratios[randint(0,3)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(3,11)], dt * ratios[randint(3,11)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//
//
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(11,18)], dt * ratios[randint(11,18)]) ;
//            time += dt * ratios[j] ;
//        }
//    }

//    //*************
//    tempo = 180;
//    dt = (float)60 / tempo ;
//
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(0,3)], dt * ratios[randint(0,3)]) ;
//            sinQ (s, time, freq * ratios[randint(3,9)], dt * ratios[randint(3,9)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(3,11)], dt * ratios[randint(3,11)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//    
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(11,18)], dt * ratios[randint(11,18)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
////********************
//    tempo = 270;
//    dt = (float)60 / tempo ;
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(0,3)], dt * ratios[randint(0,3)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(3,11)], dt * ratios[randint(3,11)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
//    
//    
//    for (int j=0; j<12; j++) {
//        for (int i=0; i<=numi[j]; i++) {
//            sinQ (s, time, freq * ratios[randint(11,18)], dt * ratios[randint(11,18)]) ;
//            time += dt * ratios[j] ;
//        }
//    }
////*****************
    
//    for (int i=0; i<=3; i++) {
//        sinQ (s, time, freq *d tArr[j], dt * dtArr[j]) ;
//        time += dt3;
//    }
//
//    for (int i=0; i<=3; i++) {
//        sinQ (s, time, freq * 5 * 0.25, dt4) ;
//        time += dt4;
//    }
//
//    for (int i=0; i<=3; i++) {
//        sinQ (s, time, freq * 6 * 0.2, dt5) ;
//        time += dt5;
//    }
//
//    for (int i=0; i<=3; i++) {
//        sinQ (s, time, freq * 4 * 0.333, dt6) ;
//        time += dt6;
//    }

    
    
    
    // ************************************************************************
//    amp = 0.1 ; atk = 0.1 ; dcy = 0.1 ; sus = 0.1 ;
//    tempo = 60;
//    dt = (float)60 / tempo ;
//
//    
//    time1 = time ;
//    
//    for (int j=0; j<4; j++) {
//        
//        for (int i =0; i<12; i++) {
//            len = dt * fluteLen[i];
//            s.add<OscTrm>( time1 ).set(len, fluteMel[i], amp, len * 0.1 - 0.05, len * 0.3 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
//            
//            if (j >= 1) {
//                s.add<OscTrm>( time1 ).set(len, fluteMel2[i], amp, len * 0.1 - 0.05, len * 0.3 , sus, 0.4,4,8,0.5, tbSqr, 0.8);
//            }
//            
//            time1 += len;
//        }
//        float temp = time1;
//        if (j > 1) {
//            for (int i=0; i<15; i++) {
//                freq = fluteMel[i%6] + 1 ; len = 0.5 ;
////                sinQ (s, temp, freq, len) ;
//                s.add<SineEnv>( temp ).set( len, freq, 0.3, 0.03, 0.03);
//                temp += len;
//            }
//
//        }
//    }
//    
//    for (int i=0; i<15; i++) {
//        freq = fluteMel[randint(1,7)] + 1 ; len = rand(0.3, 1.4) ;
//        sinQ (s, time1, freq, len) ;
//
////        s.add<SineEnv>( time1 ).set( len, freq, 0.2, 0.03, 0.03);
//        time1 += len;
//    }
//    
//    //s.add<SineEnv>( time1 ).set( len, freq, 0.3, atk, dcy);
//    
//    // START BASS
//    for (int i = 0; i<4; i++) {
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], 0.3, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], 0.3, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        time += dt * 4;
//    }
//    
//    
//    for (int i = 0; i<4; i++) {
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4], amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4] * 0.5, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[i%4] * 0.5, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        time += dt * 4;
//    }
//    
//    int note = 0;
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    time += dt * 4;
//    note++;
//    
//    sus = 0.05 ;
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    time += dt * 4;
//    note++;
//    
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//    time += dt * 4;
//    note++;
//    
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//    s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//    time += dt * 4;
//    
//    note = 0;
//    
//    saveTime = time ;
//    
//    for (int i = 0; i<4; i++) {
//        note = 0;
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.6667, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        time += dt * 4;
//        note++;
//        
//        sus = 0.05 ;
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.8, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        time += dt * 4;
//        note++;
//        
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.75, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 0.05, 0.5 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.55, amp, 3   , 0.05, 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//        time += dt * 4;
//        note++;
//        
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note], amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.5, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[note] * 0.25, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 0.05, 0.5 , sus, 0.4,8,4,0.5, tbSin, 0.8);
//        s.add<OscTrm>( time ).set(dt * 4, bassline[0] * 0.6667, amp, 3   , 0.05, sus, 0.4,8,4,0.5, tbSin, 0.8);
//        time += dt * 4;
//        
//        note = 0;
//        
//    }
//    // END BASS
//    
//    saveTime2 = time ;
//    time = saveTime + dt * 4 * 4 ;
//    
//    
//    
//    
//    // ***********************
//
//    time = saveTime2 ;
//    
//    float melody[] = { 0, 987.77, 1046.50, 1108.73, 783.99, 698.46, 523.25, 587.33 } ;
//    float melodyLen[] = {1, 4, 1, 1, 4, 4, 2, 4} ;
//    tempo = 160;
//    dt = (float)60 / tempo ;
//    
//    for (int j=0; j<5; j++) {
//        if (j%2 == 1) {
//            saveTime = time;
//            for (int i = 0; i<5; i++) {
//                len = dt * melodyLen[i] ;
//                s.add<OscTrm>( time ).set(len, melody[i], amp, len * 0.25, len * 0.25 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//                time += len;
//            }
//            saveTime = saveTime + 11. * dt ;
//            
//            int i = 5 ;
//            len = dt * melodyLen[i] ;
//            s.add<OscTrm>( saveTime ).set(len, melody[i], amp, len * 0.25, len * 0.25 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//            saveTime += len * 0.5; i++;
//            len = dt * melodyLen[i] ;
//            s.add<OscTrm>( saveTime ).set(len, melody[i], amp, len * 0.25, len * 0.25 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//            saveTime += len; i++;
//            len = dt * melodyLen[i] ;
//            s.add<OscTrm>( saveTime ).set(len, melody[i], amp, len * 0.25, len * 0.25 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//            saveTime += len;
//            time = saveTime - len;
//            
//        }
//        else {
//            for (int i = 0; i<6; i++) {
//                len = dt * melodyLen[i] ;
//                s.add<OscTrm>( time ).set(len, melody[i], amp, len * 0.25, len * 0.25 , 0.1, 0.4,4,8,0.5, tbSin, 0.8);
//                time += len;
//            }
//        }
//        time += dt;
//    }

    
//    std::cout << time << std::endl;
    
    AudioIO io(256, 44100., s.audioCB, &s);
    Sync::master().spu(io.fps());
    
    s.io().mapAudioIOData<AudioIOData>(io);
//    s.recordNRT("nlmfinal.wav", 300);
    s.recordNRT("nlm.wav", 240);

//    
//    io.start();
//    printf("\nPress 'enter' to quit...\n");  getchar();

}
