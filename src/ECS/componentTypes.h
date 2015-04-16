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
#include "entityx/Entity.h"

#define COMPONENT( X ) struct X : public entityx::Component<X>

enum BodyType {
    GRAV,
    SHIP,
    MISSILE,
    PROJECTILE,
    MAX_BODY_TYPE
};

extern glm::mat4 world;

typedef unsigned int tag_t;

COMPONENT(Tag)
{
    static tag_t previous;

    Tag()
    {
        tag = ++previous;
    }

    tag_t tag;
};

COMPONENT(MissileLogic)
{
    MissileLogic() {}
    MissileLogic(entityx::Entity p, entityx::Entity t)
    : parent(p), target(t) {}

    tag_t targettag;
    entityx::Entity target;
    tag_t parenttag;
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
    std::string debugName;
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
    std::string debugName;
    static int instanceCount;
};


COMPONENT(Weapon)
{
    Weapon() {}
    
};

enum UISelectionType
{
    INVALID,
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
    
    tag_t parenttag;
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
    
    std::vector<float> path;
    glm::mat4 transform;
    GLuint vao;
    GLuint vbo;
};

struct Animation
{
    Animation(float t) : totalTime(t) {}
    Animation(float d, float t) : delayedStartTime(d),
                                           totalTime(t)
    {}
    
    float delayedStartTime = 0;
    float elapsedTime = 0.01;
    float totalTime;
};

enum class AnimationState
{
    invalid,
    start,
    end
};

struct UIElement
{
    UIElement(float t, float r) : time(t), rotateByRadian(r) {};
    UIElement(float d, float t, float r) : time(d, t), initialRotation(r) {};
    
    bool hover = false;
    AnimationState state = AnimationState::invalid;
    Animation time;
    glm::vec2 offset2d;
    glm::vec2 scale2d;
    float initialRotation;
    float rotateByRadian;
    //size, color, animation style?
};


//GUI Circle menu consists of one center element and n leaf elements
//on activation, center animates, then leaves animate to open state
//mouseHover state
//mouseClick state
// send event to relevant system(s)
COMPONENT(GUICircleMenu)
{
    GUICircleMenu() {}
    GUICircleMenu(entityx::Entity t, int n) : target(t)
    {
        leafMenus.reserve(n);
        for (auto i = 0; i < n; i++)
        {
            leafMenus.push_back(UIElement((i+1) * 0.05 / (float)n,
                                          0.6,
                                          -M_PI/4 + i * 2.0*M_PI/(float)n));
        }
        centerElement.state = AnimationState::start;
    }
    
    float size = 50; //circular hit target (radius)
    entityx::Entity target;
    UIElement centerElement = UIElement(0.6, 0);
    std::vector<UIElement> leafMenus;
};

#endif
