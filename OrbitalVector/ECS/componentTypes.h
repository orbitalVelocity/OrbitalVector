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

#define COMPONENT( X ) struct X : public entityx::Component<X>

COMPONENT(MissileLogic)
{
    MissileLogic() {}
    MissileLogic(entityx::Entity p, entityx::Entity t)
    : parent(p), target(t) {}
    
    entityx::Entity target;
    entityx::Entity parent;
    
};

COMPONENT(PlayerControl)
{
    PlayerControl() {}
    
    std::vector<entityx::Entity> selectedEntities;
    std::vector<entityx::Entity> mouseOverEntities;
};

COMPONENT(Ship)
{
    Ship() {}
    
    int meshID;
};

COMPONENT(Missile)
{
    Missile() {}
    
    int meshID;
};


COMPONENT(Weapon)
{
    Weapon() {}
    
};

enum UISelectionType
{
    HOVER,
    SELECTED
};

COMPONENT(UISelection)
{
    UISelection() {}
    UISelection(int t) : type(t) {}
    int type;
};


COMPONENT(Velocity)
{
    Velocity() {}
    Velocity(glm::vec3 v) : vel(v) {}
    
    glm::vec3 vel;
};

COMPONENT(Position)
{
    Position() {}
    Position(glm::vec3 p) : pos(p) {}
    
    glm::vec3 pos;
};

COMPONENT(GM)
{
    GM() {}
    GM(double _gm) : gm(_gm) {}
    double gm;
};

COMPONENT(Parent)
{
    Parent() {}
    Parent(entityx::Entity::Id p) : parent(p) {}
    
    entityx::Entity::Id parent;
};

COMPONENT(OrbitalBodyType)
{
    OrbitalBodyType() {}
    OrbitalBodyType(BodyType bt) : orbitalBodyType(bt) {}
    
    BodyType orbitalBodyType;
};

COMPONENT(Orientation)
{
    Orientation () {}
    Orientation (glm::mat4 o) : orientation(o) {}
    
    glm::mat4 orientation;
    float x=0, y=90;
};

COMPONENT(Radius)
{
    Radius () {}
    Radius (float r) : radius(r) {}
    
    float radius;
};

COMPONENT(CameraComponent)
{
    CameraComponent () {}

    glm::vec3 position;
    glm::vec3 focus;
    
    float fov;
    float hAngle, vAngle;
    float nearPlane, farPlane;
    float ratio;
};

COMPONENT(OrbitPath)
{
    OrbitPath() {}
    
    vector<float> path;
    glm::mat4 transform;
};




#endif
