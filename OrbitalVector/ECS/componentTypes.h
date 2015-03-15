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
#include "entityx/Entity.h"

struct Velocity : public entityx::Component<Velocity>
{
    Velocity() {}
    Velocity(glm::vec3 v) : vel(v) {}
    
    glm::vec3 vel;
};

struct Position : public entityx::Component<Position>
{
    Position() {}
    Position(glm::vec3 p) : pos(p) {}
    
    glm::vec3 pos;
};

struct GM : public entityx::Component<GM>
{
    GM() {}
    GM(double _gm) : gm(_gm) {}
    double gm;
};

struct Parent : public entityx::Component<Parent>
{
    Parent() {}
    Parent(entityx::Entity::Id p) : parent(p) {}
    
    entityx::Entity::Id parent;
};

struct OrbitalBodyType : public entityx::Component<OrbitalBodyType>
{
    OrbitalBodyType() {}
    OrbitalBodyType(BodyType bt) : orbitalBodyType(bt) {}
    
    BodyType orbitalBodyType;
};

struct Orientation :public entityx::Component<Orientation>
{
    Orientation () {}
    Orientation (glm::mat4 o) : orientation(o) {}
    
    glm::mat4 orientation;
};

struct Radius : public entityx::Component<Radius>{
    Radius () {}
    Radius (float r) : radius(r) {}
    
    float radius;
};
#endif
