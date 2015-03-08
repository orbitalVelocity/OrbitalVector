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


class UnitManager
{
public:
    std::vector<glm::vec3> vPos, vVel;
    std::vector<double> vGM;
    std::vector<Spatial> vSpatial;
    std::vector<Unit> vUnit;
    
public:
    UnitManager(){}
    
    void addUnit();
};
#endif /* defined(__OrbitalVector__unitManager__) */
