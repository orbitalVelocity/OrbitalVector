//
//  user.cpp
//  OrbitalVector
//
//  Created by Si Li on 11/5/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "user.h"
#include <math.h>

User::User(bool test)
{
    isUser = test;
    bulletCount = 0;
    bulletMass = 0;
    missileCount = 4;
    missileMass = 2e4;
    
    mainFuel = 600;
    fuelMass = 1;
    mainISP = 800;
    rcsFuel = 0;
    rcsMass = 0;
    rcsISP = 0;
    
    hp = 100;
    dryMass = 2e5;
    
    target = -1;
}

float User::getDeltaV()
{
    float dv;
    auto fullMass = dryMass + mainFuel * fuelMass
                  + rcsFuel * rcsMass
                  + bulletCount * bulletMass
                  + missileCount * missileMass;
    dv = mainISP * log(fullMass/dryMass);
    return dv;
}

void update(vector<ActionType> &actionList)
{
    for (const auto& action : actionList)
    {
        //big switch statement
    }
}

void update(UserInput &input)
{
    //read mouse input
}