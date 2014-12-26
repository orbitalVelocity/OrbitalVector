//
//  rk547m.cpp
//  SpaceSimulator
//
//  Created by Si Li on 3/31/14.
//  Copyright (c) 2014 PLH. All rights reserved.
//

#include "rk547m.h"
#include <vector>
using namespace std;

#define OLDWAY 0
#define MIN_DT .1

vector<vector<state> > ks;
vector<body> sys;
vector<float> orbits;
glm::mat4 world;

#if OLDWAY
int numBodyPerType[MAX_BODY_TYPE];
int sysIndexOffset[MAX_BODY_TYPE];
#endif

void printks(vector<vector<state>> &ks)
{
    
    for (auto &k : ks)
        for (auto &s : k)
        {
            cout << printVec3(s.pos) << " v: " << printVec3(s.vel) << "\n";
        }
}

void printsys(vector<body> &sys)
{
    for (auto &body : sys)
    {
        cout << "p: " << printVec3(body.sn.pos) << " v: " << printVec3(body.sn.vel) << "\n";
    }
}

#if OLDWAY
//remove the del'th item of type in sys
void removeFromSys(int del, int type)
{
    assert(type != BodyType::GRAV && "grav wells are indistructable at this time");
    auto offset = sysIndexOffset[type] + del;
    debug_cout("erasing body from sys: " + to_string(offset));
    sys.erase(sys.begin()+offset);
    numBodyPerType[type] = (BodyType)(numBodyPerType[type] - 1);
    updateSysIndexOffset();
}

//remove the item directly
void removeFromSys(vector<body>::iterator it)
{
    //find type from it
    int type;
    for (int i=0; i < BodyType::MAX_BODY_TYPE; i++) {
        if (sysIndexOffset[i] + numBodyPerType[i] > it - sys.begin()) {
            type = i;
            break;
        }
    }
    
    debug_cout("erasing body of type from sys: " + to_string(type));
    sys.erase(it);
    numBodyPerType[type] = (BodyType)(numBodyPerType[type] - 1);
    updateSysIndexOffset();
}

void InsertToSys(body &body, int type)
{
    auto offset = sysIndexOffset[type] + numBodyPerType[type];
    assert(sys.size() >= offset && "trying to insert past vector size");
    debug_cout("inserting to sys: " + to_string(offset));
    sys.insert(sys.begin() + offset, body);
    numBodyPerType[type] += (BodyType)(1);
    updateSysIndexOffset();
    cout << "inserted to offset: " << offset << endl;
}

void updateSysIndexOffset()
{
    auto total = 0;
    for (int i = 0; i < MAX_BODY_TYPE; i++)
    {
        sysIndexOffset[i] = total;
        total += numBodyPerType[i];
    }
    
    for (auto &k : ks)
    {
        k.clear();
        k.resize(sys.size());
    }
}

body& getBody(int bodyNum, int type)
{
    return sys[sysIndexOffset[type] + bodyNum];
}

body& getShipBody(int bodyNum)
{
    return sys[sysIndexOffset[BodyType::SHIP] + bodyNum];
}
#endif

void markForDeletion(vector<body> &sys, vector<bool> &markedForRemoval)
{
    //mark elements that needs to be removed
    //TODO: due to collision, too far/escape velocity
    markedForRemoval.resize(sys.size(), false);
    
    for (int i=0; i < sys.size(); i++) {
        auto collided = false;
        for (int j = i+1; j < sys.size(); j++) { //skip element 0 - planet
            if (i == j ) {
                continue;
            }
            auto dist = glm::length(sys[i].sn.pos - sys[j].sn.pos);
            auto minDist = sys[i].radius + sys[j].radius;
            if (dist <= minDist) {
                markedForRemoval[j] = true;
                //only mark if not a grav well
                markedForRemoval[i] = true;
                collided = true;
            }
        }
        if (collided) {
            debug_cout("collision is happening!");
        }
        if (not markedForRemoval[i] && glm::length(sys[i].sn.pos - sys[0].sn.pos) > 400)
        {
            markedForRemoval[i] = true;
            cout << "body " << i << " has left the building\n";
        }
    }
}

void orbitDelta(GLfloat &delta,
                vector<vector<state> > &ks, vector<body> &sys,
                bool changeDT)
{
    
    float absTol = 0;
    float relTol = 1e-6;
    int nBodies = (int)sys.size();
    for (int i = 0; i < nBodies; i++)
        sys[i].si = sys[i].sn;
    
#if 0
        for (int i=0; i<nBodies; i++) {
            cout << "body: " << i << " pos: ";
            printVec3(sys[i].sn.pos);
            cout << "\tvel: ";
            printVec3(sys[i].sn.vel);
        }
#endif
    RK45Int(delta, absTol, relTol,
            ks, sys, changeDT);
    
}

//with RK5(4)7M code adapted from Matthew Roesle
void calcRHS(float t, vector<body> &sys) {
    float r, tmp;
    int i, j;
    int nBodies = (int)sys.size();
    
    //    nRHSCalls ++;
    
    for (i = 0; i < nBodies; i++) {
        state &RHS = sys[i].RHS;
        state &stmp = sys[i].stmp;
        RHS.pos = stmp.vel;
        RHS.vel = glm::vec3(0);
    }
    
    for (i = 0; i < nBodies-1; i++) {
        state &si = sys[i].stmp;
        state &RHSi = sys[i].RHS;
        
        for (j = i+1; j < nBodies; j++) {
            state &sj = sys[j].stmp;
            state &RHSj = sys[j].RHS;
            
            glm::vec3 dpos = si.pos - sj.pos;
            r = 1.0/glm::length2(dpos);// magSqr();
            r *= sqrt(r);
            
            tmp = sys[j].mu*r;
            
            RHSi.vel -= dpos*tmp;
            
            tmp = sys[i].mu*r;
            
            RHSj.vel += dpos*tmp;
        }
    }
}

void RK45Int(float &dt, float absTol, float relTol,
             vector<vector<state> > &ks, vector<body> &sys,
             bool changeDT) {
    
    float t = 0;
    state *k1 = ks[0].data();
    state *k2 = ks[1].data();
    state *k3 = ks[2].data();
    state *k4 = ks[3].data();
    state *k5 = ks[4].data();
    state *k6 = ks[5].data();
    state *k7 = ks[6].data();
    state *sn5 = ks[7].data();
    
    int nBodies = (int)sys.size();
    
    
    
    state sn4;
    const float A21 = 0.2, A31 = 0.075, A32 = 0.225, A41 = 44.0/45.0;
    const float A42 = -56.0/15.0, A43 = 32.0/9.0, A51 = 19372.0/6561.0;
    const float A52 = -25360.0/2187.0, A53 = 64448.0/6561.0, A54 = -212.0/729.0;
    const float A61 = 9017.0/3168.0, A62 = -355.0/33.0, A63 = 46732.0/5247.0;
    const float A64 = 49.0/176.0, A65 = -5103.0/18656.0;
    const float B11 = 35.0/384.0, B13 = 500.0/1113.0, B14 = 125.0/192.0;
    const float B15 = -2187.0/6784.0, B16 = 11.0/84.0;
    const float B21 = 5179.0/57600.0, B23 = 7571.0/16695.0, B24 = 393.0/640.0;
    const float B25 = -92097.0/339200.0, B26 = 187.0/2100.0, B27 = 0.025;
    const float C2 = 0.2, C3 = 0.3, C4 = 0.8, C5 = 8.0/9.0, C6 = 1.0, C7 = 1.0;
    float t1, t2, t3, t4, t5, t6, t7;
    float err, maxerr = 0.0;
    int i;
    
    for (i = 0; i < nBodies; i++)
        sys[i].stmp = sys[i].si;
    
    calcRHS(t, sys);
    
    t1 = A21*dt;
    for (i = 0; i < nBodies; i++) {
        k1[i] = sys[i].RHS;
        sys[i].stmp = sys[i].si + k1[i]*t1;
//        printVec3(sys[i].stmp.pos); cout << " = "; printVec3(sys[i].si.pos);
    }
    
    calcRHS(t + C2*dt, sys);
    
    t1 = A31*dt;
    t2 = A32*dt;
    for (i = 0; i < nBodies; i++) {
        k2[i] = sys[i].RHS;
        sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2;
    }
    
    calcRHS(t + C3*dt, sys);
    
    t1 = A41*dt;
    t2 = A42*dt;
    t3 = A43*dt;
    for (i = 0; i < nBodies; i++) {
        k3[i] = sys[i].RHS;
        sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3;
    }
    
    calcRHS(t + C4*dt, sys);
    
    t1 = A51*dt;
    t2 = A52*dt;
    t3 = A53*dt;
    t4 = A54*dt;
    for (i = 0; i < nBodies; i++) {
        k4[i] = sys[i].RHS;
        sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3 + k4[i]*t4;
    }
    
    calcRHS(t + C5*dt, sys);
    
    t1 = A61*dt;
    t2 = A62*dt;
    t3 = A63*dt;
    t4 = A64*dt;
    t5 = A65*dt;
    for (i = 0; i < nBodies; i++) {
        k5[i] = sys[i].RHS;
        sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3 + k4[i]*t4 + k5[i]*t5;
    }
    
    calcRHS(t + C6*dt, sys);
    
    t1 = B11*dt;
    t3 = B13*dt;
    t4 = B14*dt;
    t5 = B15*dt;
    t6 = B16*dt;
    for (i = 0; i < nBodies; i++) {
        k6[i] = sys[i].RHS;
        sn5[i] = sys[i].si + k1[i]*t1 + k3[i]*t3 + k4[i]*t4 + k5[i]*t5 + k6[i]*t6;
        sys[i].stmp = sn5[i];
    }
    
    calcRHS(t + C7*dt, sys);
    
    t1 = B21*dt;
    t3 = B23*dt;
    t4 = B24*dt;
    t5 = B25*dt;
    t6 = B26*dt;
    t7 = B27*dt;
    for (i = 0; i < nBodies; i++) {
        k7[i] = sys[i].RHS;
        sn4.pos = sys[i].si.pos + k1[i].pos*t1 + k3[i].pos*t3 + k4[i].pos*t4 + k5[i].pos*t5 + k6[i].pos*t6 + k7[i].pos*t7;
        
        glm::vec3 e = sn5[i].pos - sn4.pos;
        err = glm::length(e) / (absTol + relTol * glm::length(sn5[i].pos));
        if (err > maxerr)
            maxerr = err;
    }
    
    if ((maxerr < 1.0) || (dt == 1.0)) {
        t += dt;
        if (maxerr != 0.0)
            dt = 0.9*dt*pow(maxerr, -0.20);
        else
            dt *= 2.0;
        if (dt < MIN_DT)
            dt = MIN_DT;
    }
    else {
        if (changeDT) {
            if (maxerr != 0.0)
                dt = 0.9*dt*pow(maxerr, -0.20);
            else
                dt *= 0.5;
            if (dt < MIN_DT)
                dt = MIN_DT;
        }
        
        
        for (i = 0; i < nBodies; i++)
            sys[i].stmp = sys[i].si;
        
        calcRHS(t, sys);
        
        t1 = A21*dt;
        for (i = 0; i < nBodies; i++) {
            k1[i] = sys[i].RHS;
            sys[i].stmp = sys[i].si + k1[i]*t1;
        }
        
        calcRHS(t + C2*dt, sys);
        
        t1 = A31*dt;
        t2 = A32*dt;
        for (i = 0; i < nBodies; i++) {
            k2[i] = sys[i].RHS;
            sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2;
        }
        
        calcRHS(t + C3*dt, sys);
        
        t1 = A41*dt;
        t2 = A42*dt;
        t3 = A43*dt;
        for (i = 0; i < nBodies; i++) {
            k3[i] = sys[i].RHS;
            sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3;
        }
        
        calcRHS(t + C4*dt, sys);
        
        t1 = A51*dt;
        t2 = A52*dt;
        t3 = A53*dt;
        t4 = A54*dt;
        for (i = 0; i < nBodies; i++) {
            k4[i] = sys[i].RHS;
            sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3 + k4[i]*t4;
        }
        
        calcRHS(t + C5*dt, sys);
        
        t1 = A61*dt;
        t2 = A62*dt;
        t3 = A63*dt;
        t4 = A64*dt;
        t5 = A65*dt;
        for (i = 0; i < nBodies; i++) {
            k5[i] = sys[i].RHS;
            sys[i].stmp = sys[i].si + k1[i]*t1 + k2[i]*t2 + k3[i]*t3 + k4[i]*t4 + k5[i]*t5;
        }
        
        calcRHS(t + C6*dt, sys);
        
        t1 = B11*dt;
        t3 = B13*dt;
        t4 = B14*dt;
        t5 = B15*dt;
        t6 = B16*dt;
        for (i = 0; i < nBodies; i++) {
            k6[i] = sys[i].RHS;
            sn5[i]= sys[i].si + k1[i]*t1 + k3[i]*t3 + k4[i]*t4 + k5[i]*t5 + k6[i]*t6;
            sys[i].stmp = sn5[i];
        }
        
        calcRHS(t + C7*dt, sys);
        
        t1 = B21*dt;
        t3 = B23*dt;
        t4 = B24*dt;
        t5 = B25*dt;
        t6 = B26*dt;
        t7 = B27*dt;
        for (i = 0; i < nBodies; i++) {
            k7[i] = sys[i].RHS;
            sn4.pos = sys[i].si.pos + k1[i].pos*t1 + k3[i].pos*t3 + k4[i].pos*t4 + k5[i].pos*t5 + k6[i].pos*t6 + k7[i].pos*t7;
            
            glm::vec3 e = sn5[i].pos - sn4.pos;
            err = glm::length(e) / (absTol + relTol * glm::length(sn5[i].pos));
            if (err > maxerr)
                maxerr = err;
        }
        
        t += dt;
        
        if (maxerr != 0.0)
            dt = 0.9*dt*pow(maxerr, -0.20);
        else
            dt *= 2.0;
        if (dt < MIN_DT)
            dt = MIN_DT;
    }
    
    for (i = 0; i < nBodies; i++)
        sys[i].sn = sn5[i];
}