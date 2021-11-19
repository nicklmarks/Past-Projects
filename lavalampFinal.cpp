// Nicholas Marks
// Mat201 Final Project
// Due: 12/16/2013

#include <iostream>
#include "allocore/io/al_App.hpp"
#include "alloutil/al_BroadcastOmniApp.hpp"
#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"

using namespace std;
using namespace al;
using namespace rnd;

#define N 30

#define startFreqOffset 0
#define MAXY 10

#define StartRadius 0.5
#define MAXRadius 7
#define StartTemp 50
#define StartDist 10

#define MAXDist 12
#define MINDist 1
#define horizontalMovementConst 0.1
#define MovementDamp 0.01
#define MaxRest 500

float aScale[] = { 130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 195.0, 207.7, 220, 233.1, 246.9} ;

struct Agent {
    
    Vec3f position;
    Color color;
    float radius, frequency, temperature;
    bool rise, fall ;
    int rest, id;
    
    Agent() {
        
    }
    /*
    void upDirection() {
        float temp = uniformS();
        if (position.y <= (-MAXY + 3)) {
            if (temp >= 0.5 && rest > MaxRest) { rise = true ; rest = 0; }
            else rise = false;
        }
        if (position.y >= (MAXY - 1)  && rest > MaxRest ) {
            if (temp >= 0.0) { fall = true ; rest = 0; }
            else fall = false;
        }
    }
     */
    
    // update the temperature
    void upTemp(int n) {
        // upper and lower bounds
        if (temperature < 0) {
            temperature = 0 ;
        }
        if (temperature > 100) {
            temperature = 100 ;
        }
        // if the agent is higher lower the temp
        if (position.y > MAXY ) {
            temperature -= ( uniformS() + 1) ;
        }
        else if (position.y >= 8 && position.y < 10) {
            temperature -= ( uniformS() ) ;
        }
        // if the agent is lower raise the temp
        else if (position.y >= -10 && position.y < -8) {
            temperature += ( uniformS() + 1) * 5 ;
        }
        else if (position.y < -MAXY ) {
            temperature += ( uniformS() + 1) ;
        }
        else {
            // add a bit of noise and randomness to the temp to get more varied movements
            if ( n < 500) {
                temperature -= uniformS() + 1 ;
            }
            else {
                if ( n % 3 == 0 ){
                    temperature -= uniformS() ;
                }
                else {
                    temperature -= uniformS() + 1 ;
                }
            }
        }
    }
    
    // update the position vector
    void upPos(/*Vec3f p*/) {
        float changeX, changeZ ;
        //bounds check
        if (position.y < -MAXY) {
            rest ++;
            position.y = -MAXY ;
        }
        if (position.y > MAXY) {
            rest++;
            position.y = MAXY ;
        }
        // x and z direction become more noisy as they move away from the origin
        if (position.y > 2) {
            changeX = uniform(-horizontalMovementConst * position.y, horizontalMovementConst * position.y) ;
            changeZ = uniform(-horizontalMovementConst * position.y, horizontalMovementConst * position.y) ;
        }
        else if (position.y < -4) {
            changeX = uniform(-horizontalMovementConst * position.y, horizontalMovementConst * position.y) ;
            changeZ = uniform(-horizontalMovementConst * position.y, horizontalMovementConst * position.y) ;
            
            changeX = changeX * .75 ;
            changeZ = changeZ * .75 ;
        }
        else {
            changeX = uniform(-horizontalMovementConst , horizontalMovementConst ) ;
            changeZ = uniform(-horizontalMovementConst , horizontalMovementConst ) ;
        }
        
        float temp = (50 - temperature) ;        
        position -= Vec3f( changeX , temp * MovementDamp, changeZ ) ;

    }
    
    // set the color based on position, flash ID color every 100 frames
    void upColor(int n) {
        float col =  (position.y + 10 ) / 30 ;
        if ( n % 100 == 0) {
            color = HSV((float)id / N, 1, 1);
        }
        else {
            color = HSV(col, 1, 1);
        }
    }
    
    // set the frequency and the radius
    void update() {
        
        frequency = temperature * 4 + 100 ;
        
        if (frequency < 200) {
            frequency = 200 ;
        }
        if (frequency > 1600) {
            frequency = 1600 ;
        }
        /// different formulas for changing size based on positive or negative Y
        if (position.y > 0) {
            radius = StartRadius + (position.y * position.y * 0.1) ;
            if ( radius > MAXRadius ) {
                radius = MAXRadius;
            }
        }
        else {
            radius = StartRadius + (position.y * position.y * 0.01) ;
        }
        
    }
    
};


struct State {
    unsigned n;
    Pose pose;
    Agent agents[N];
    Agent blue, red;
};

// BroadcastApp has a few pure virtual methods that you must
// implement: onSimulatorInit, onSimulate, onRendererLocal, and onDraw.
//
struct MyBroadcastedStateApp : BroadcastApp<State> {
    
    // Member variables declared here are machine-local copies. They will not be
    // broadcast to rendering machines. If you want to send data to rendering
    // machines, put that data in your state struct. Only use these variables for
    // machine-local stuff.
    //
    Mesh m;
    gam::Sine<> sine[N];
    
    bool shouldReset;
    
    // This is a machine-local constructor. It will only initialize and do things
    // on the local machine.
    //
    MyBroadcastedStateApp() {
        shouldReset = false;
        cout << "my State is " << sizeof(State) << " bytes\n";
        
        addSphere(m);
        m.primitive(Graphics::TRIANGLES);
        m.generateNormals();
    }
    
    // This method will be called once on the simulation machine after the
    // constructor but before "things get going". It will not be called on any
    // rendering machines.
    //
    void onSimulatorInit(State& state) {
        
        nav().pos(0, 0, 0);
        state.pose = nav();
        
        state.n = 0;
        
        state.blue.position = Vec3f( 0, 60, 0 );
        state.blue.color = Color(0, 0, 1);
        state.blue.radius = 50 ;
        
        state.red.position = Vec3f( 0, -60, 0 );
        state.red.color = Color(1, 0, 0);
        state.red.radius = 50 ;
        
        for (int i = 0; i < N; ++i) {
            state.agents[i].id = i;
            state.agents[i].position = Vec3f(sin(i) * StartDist,  0 /*-10 + (i / 20) */, -cos(i) * StartDist );
            state.agents[i].color = HSV((float)i / N, 1, 1);
            state.agents[i].radius = StartRadius;
            state.agents[i].frequency = (aScale[i%11] + startFreqOffset) * 2 ;
            state.agents[i].temperature = StartTemp ;
            state.agents[i].rise = false;
        }
    }
    
    // This method will be called on the simulation machine, be it your laptop or
    // one of the sphere machines that is not responsible for graphics rendering.
    // In this method, you will do some sort of simulation (possibly reading from
    // your state), then *write* to the state variable. To be clear, when state
    // is passed into this method, it is identical to the last state you wrote.
    //
    void onSimulate(double dt, State& state) {
        
        state.pose = nav();
        state.n++;
        
        if (state.n % 5 == 0 ) {
            for (int i = 0; i < N; ++i) {
//                state.agents[i].upDirection();
                state.agents[i].upTemp(state.n);
                state.agents[i].upPos();
                state.agents[i].upColor(state.n);
                state.agents[i].update();
            }
        }
        
    }
    
    // This method will be called on a rendering machine, be it your laptop or
    // any/all of the AlloSphere rendering machines. Use this method to upsample,
    // decorate, filter, decompress or otherwise interpret your state data before
    // it gets drawn in onDraw.
    //
    void onRendererLocal(double dt, State& state, bool gotMostRecentState) {
        
        pose = state.pose;
    }
    
    // This method will be called on a rendering machine, be it your laptop or
    // any/all of the AlloSphere rendering machines. Use this method to draw your
    // state. Only do drawing because this method may be called *many* times.
    //
    void onDraw(Graphics& g, const State& state) {
        
        // Here we do not use our passed-in state directly, because we want to take
        // the opportunity to translate our state data into something drawable. If
        // we did that step here, we'd be doing it 12 times instead of just once.
        //
        for (int i = 0; i < N; ++i) {
            g.pushMatrix();
            g.color(state.agents[i].color);
            g.translate(state.agents[i].position);
            g.scale(state.agents[i].radius);
            g.draw(m);
            g.popMatrix();
        }
        
        g.pushMatrix();
        g.color(state.blue.color);
        g.translate(state.blue.position);
        g.scale(state.blue.radius);
        g.draw(m);
        g.popMatrix();
        
        g.pushMatrix();
        g.color(state.red.color);
        g.translate(state.red.position);
        g.scale(state.red.radius);
        g.draw(m);
        g.popMatrix();
        
        
    }
    
    virtual void onSound(AudioIOData& io, const State& state) {
        gam::Sync::master().spu(audioIO().fps());
        for (int i = 0; i < N; ++i) {
            sine[i].freq(state.agents[i].frequency);
        }
        while (io()) {
            float s = 0;
            for (int i = 0; i < N; ++i) {
                float temp = (state.pose.pos() - state.agents[i].position).mag() ;
                if (temp < 1) {
                    temp = 1;
                }
                s += sine[i]() / temp;
            }
            io.out(0) = io.out(1) = s / N;
        }
    }
    
    virtual bool onKeyDown(const Keyboard& k) {
        shouldReset = (k.key() == ' ');
        return true;
    }
};

int main() { MyBroadcastedStateApp().start(); }
