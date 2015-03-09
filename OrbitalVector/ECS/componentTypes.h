//
//  componentTypes.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_componentTypes_h
#define OrbitalVector_componentTypes_h

#include "rk547m.h"
#include "includes.h"

struct Velocity {
    Velocity() {}
    Velocity(glm::vec3 v) : vel(v) {}
    
    glm::vec3 vel;
};

struct Position {
    Position() {}
    Position(glm::vec3 p) : pos(p) {}
    
    glm::vec3 pos;
};

struct GM {
    GM() {}
    GM(double _gm) : gm(_gm) {}
    double gm;
};

struct Parent {
    Parent() {}
    Parent(entityx::Entity::Id p) : parent(p) {}
    
    entityx::Entity::Id parent;
};

struct OrbitalBodyType {
    OrbitalBodyType() {}
    OrbitalBodyType(BodyType bt) : orbitalBodyType(bt) {}
    
    BodyType orbitalBodyType;
};

struct Orientation {
    Orientation () {}
    Orientation (glm::mat4 o) : orientation(o) {}
    
    glm::mat4 orientation;
};

struct Radius {
    Radius () {}
    Radius (float r) : radius(r) {}
    
    float radius;
};
#endif
