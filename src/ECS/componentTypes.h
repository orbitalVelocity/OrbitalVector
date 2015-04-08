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
    bool done = false;
    
};

COMPONENT(PlayerControl)
{
    PlayerControl() {}
    
    std::vector<entityx::Entity> selectedEntities;
    std::vector<entityx::Entity> mouseOverEntities;
};

COMPONENT(Ship)
{
    Ship()
    {
        debugName = "ship " + std::to_string(instanceCount);
        instanceCount++;
    }
    
    int meshID;
    string debugName;
    static int instanceCount;
};


COMPONENT(Missile)
{
    Missile()
    {
        debugName = "missile" + std::to_string(instanceCount);
        instanceCount++;
    }
    int meshID;
    string debugName;
    static int instanceCount;
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
    OrbitPath() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }
    
    ~OrbitPath() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    
    vector<float> path;
    glm::mat4 transform;
    GLuint vao;
    GLuint vbo;
};

struct Animation
{
    Animation() {}
    Animation(float t) : totalTime(t) {}
    
    float elapsedTime = 0;
    float totalTime;
};

struct CenterElement
{
    CenterElement() {};
    
    Animation time;
};
//GUI Circle menu consists of one center element and n leaf elements
//on activation, center animates, then leaves animate to open state
//mouseHover state
//mouseClick state
// send event to relevant system(s)
COMPONENT(GUICircleMenu)
{
    GUICircleMenu() {}
    GUICircleMenu(entityx::Entity t, int n) : target(t), numberOfLeaves(n) {}
    
    entityx::Entity target;
    glm::vec2 screenPos;
    int state;
    int numberOfLeaves;
};

#endif
