//
//  ecs.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_ECS_H
#define OrbitalVector_ECS_H
#include "includes.h"
#include "rk547m.h"

#define OLDECS true
using namespace glm;
const int SHIPOFFSET = 1;

vec3 getShipPos(int index)
{
#if OLDECS
    return sys[index+SHIPOFFSET].sn.pos;
#else
    return entityx::entity.get<POSITION>(index);
#endif
}

#endif
