//
//  componentTypes.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_componentTypes_h
#define OrbitalVector_componentTypes_h

#include "includes.h"

using namespace glm;

struct Velocity {
    Velocity() {}
    Velocity(vec3 p) : pos(p) {}
    
    vec3 pos;
};

struct Position {
    Position() {}
    Position(vec3 p) : pos(p) {}
    
    vec3 pos;
};
#endif
