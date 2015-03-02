//
//  rk547m.h
//  SpaceSimulator
//
//  Created by Si Li on 3/31/14.
//  Copyright (c) 2014 PLH. All rights reserved.
//

#ifndef __SpaceSimulator__rk547m__
#define __SpaceSimulator__rk547m__

#include <iostream>
#include "includes.h"
#include <math.h>

using namespace std;

static GLfloat gConstant = 6.67e-11;
static GLfloat tolerance = 1e2;


class state {        // State of an orbiting object -- position and velocity
public:
    state() {}
    state(glm::vec3 p) : pos(p) {}
    state(glm::vec3 p, glm::vec3 v) : pos(p), vel(v) {}
    
    state operator+ (const state& rhs) const
    {
        return state(pos + rhs.pos, vel + rhs.vel);
    }
    state operator* (const state& rhs) const
    {
        return state(vel * rhs.vel, vel * rhs.vel);
    }
    state operator* (const float a) const
    {
        return state(pos * a, vel * a);
    }
    state& operator= (const state& rhs)
    {
        pos = rhs.pos;
        vel = rhs.vel;
        return *this;
    }
    
    
public:
    glm::vec3 pos, vel;
};

enum BodyType {
    GRAV,
    SHIP,
    MISSILE,
    PROJECTILE,
    MAX_BODY_TYPE
};

class body {         // State of a planetary body
public:
    body() {};
    body(state s, float _mu, float r, body* p, BodyType t) :
    sn(s), mu(_mu), radius(r), parent(p), type(t), deltaV(0) {
        if (nullptr == parent) {
            soi = INFINITY;
        } else {
            //                soi = r * pow(mu/parent->mu, 2.0f/5.0f);
            soi = 66000000;
        }
        soi = 66000000;
        //cout << "soi: " << soi << "\n";
        deltaVCapacity = 5000;
    }
    
    void setParent(body* p) {
        parent = p;
    }
    void incCustom(GLfloat dv, glm::vec3 v) {
        sn.vel += glm::normalize(v) * dv;
    }
    void incPrograde(GLfloat dv) {
        GLfloat h = glm::length(sn.vel);// .mag();
        sn.vel += sn.vel * dv/h;
        deltaV += fabs(dv);
    }
    void incRadial(GLfloat dv) {
        glm::vec3 inward = getRadial();
        GLfloat h = glm::length(inward);
        sn.vel += inward * dv/h;
        deltaV += fabs(dv);
    }
    glm::vec3 cross(glm::vec3 u, glm::vec3 v) {
        return glm::vec3(u.y*v.z-u.z*v.y,
                   u.z*v.x-u.x*v.z,
                   u.x*v.y-u.y*v.x);
    }
    void incNormal(GLfloat dv) {
        glm::vec3 normal = getNormal();
        GLfloat h = glm::length(normal);
        sn.vel += normal * dv/h;
        deltaV += fabs(dv);
    }
    inline glm::vec3 getRadial() {
        return parent->sn.pos- sn.pos;
    }
    inline glm::vec3 getNormal() {
        glm::vec3 inward = getRadial();
        return cross(inward, sn.vel);
    }
    inline glm::vec3 getPrograde() {
        return sn.vel;
    }
public:
    body* parent;
    state si, sn;       // Previous and current state of the body - position and velocity
    state RHS, stmp;    // RHS for the body and temporary state
    float mu;          // Gravitational constant of the body
    float radius;
    float soi;
    float deltaV;
    GLfloat deltaVCapacity;
    BodyType type;
};

/**
 Dumps pos/vel data of entire system
 @param sys    all bodies in system
 */
void printsys(vector<body> &sys);
void printks(vector<vector<state>> &ks);
void orbitPhysicsUpdate(GLfloat &delta,
                vector<vector<state> > &ks, vector<body> &sys,
                bool variableDT);

void calcRHS(float t);
void RK45Int(float &dt, float absTol, float relTol,
             vector<vector<state> > &ks, vector<body> &sys,
             bool varialbeDT);

void markForDeletion(vector<body> &sys, vector<bool> &markedForRemoval);
extern glm::mat4 world;
extern vector<vector<state> > ks;           //state of system
extern vector<body> sys;                    //sys[planetID].sn is pos
extern vector<float> orbits;   //orbits[planetID] is path
                                            //of planets'/satellites'
                                            //trajectory


extern int numBodyPerType[MAX_BODY_TYPE];
extern int sysIndexOffset[MAX_BODY_TYPE];

void removeFromSys(int del, int type);
void removeFromSys(vector<body>::iterator it);
void InsertToSys(body &body, int type);
void updateSysIndexOffset();

body& getBody(int shipNum, int type);



#endif /* defined(__SpaceSimulator__rk547m__) */
