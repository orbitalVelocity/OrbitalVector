//
//  unitManager.h
//  OrbitalVector
//
//  Created by Si Li on 11/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__unitManager__
#define __OrbitalVector__unitManager__

#include <iostream>
#include "includes.h"
#include "spatial.h"
#include "unit.h"

using namespace std;
using namespace glm;

class UnitManager
{
public:
    vector<vec3> vPos, vVel;
    vector<double> vGM;
    vector<Spatial> vSpatial;
    vector<Unit> vUnit;
    
public:
    UnitManager(){}
    
    void addUnit();
};
#endif /* defined(__OrbitalVector__unitManager__) */
