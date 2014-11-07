//
//  user.h
//  OrbitalVector
//
//  Created by Si Li on 11/5/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//
// maintains user states: ammo, fuel, health, etc
// receives actions: hit, targed, observed, etc
// sends actions: shoot, die, move, etc

#ifndef __OrbitalVector__user__
#define __OrbitalVector__user__

#include <iostream>
#include "input.h"


class User
{
public:
    User() {}
    //configuration initialization
    User(bool);
    void update(vector<ActionType> &);  //update from gameLogic
    void update(UserInput &);           //update from UserInput
    int getBulletCount()    { return bulletCount; }
    int getMissileCount()   { return missileCount; }
    float getDeltaV();
public:
    bool isUser;
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
    //tracking
    int target; //shipID
    
};
#endif /* defined(__OrbitalVector__user__) */
