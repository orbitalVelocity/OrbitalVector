//
//  unit.h
//  OrbitalVector
//
//  Created by Si Li on 11/5/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//
// maintains unit states: ammo, fuel, health, etc
// receives actions: hit, targed, observed, etc
// sends actions: shoot, die, move, etc

#ifndef __OrbitalVector__unit__
#define __OrbitalVector__unit__

#include <iostream>
#include "includes.h"
#include "input.h"

using namespace std;


class Unit
{
public:
    Unit() {}
    //configuration initialization
    Unit(bool);
    void update(vector<ActionType> &);  //update from gameLogic
    void update(UserInput &);           //update from UnitInput
    int getBulletCount()    { return bulletCount; }
    int getMissileCount()   { return missileCount; }
    float getDeltaV();
public:
    bool isUnit;
    //ammo
    int bulletCount;
    int missileCount;
    
    float bulletMass;
    float missileMass;
    
    //fuel
    float mainFuel;
    float rcsFuel;
    
    float fuelMass;
    float rcsMass;
    
    float mainISP;
    float rcsISP;
    
    //hit point
    float hp;
    
    //weight
    float dryMass;
    
    //user controlled states
    int target; //shipID
    float throttle;
    
    //animation related?
    
    
};

#endif /* defined(__OrbitalVector__unit__) */
