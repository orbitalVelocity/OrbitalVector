//
//  events.h
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_events_h
#define OrbitalVector_events_h

#include "entityx/Event.h"
#include "entityx/Entity.h"
#include "glm/glm.hpp"
#define EVENT( X ) struct X : public entityx::Event<X>

EVENT(SerializeEvent)
{
    SerializeEvent() {}
};

EVENT(UserInputEvent)
{
    UserInputEvent() {}
};

EVENT(PotentialSelectEvent)
{
    PotentialSelectEvent(entityx::Entity e)
    : entity(e)
    {}
    
    entityx::Entity entity;
};

EVENT(CollisionEvent)
{
    CollisionEvent(entityx::Entity e1, entityx::Entity e2)
    : entity1(e1), entity2(e2)
    {}
    
    entityx::Entity entity1, entity2;
};

EVENT(DeathEvent)
{
    DeathEvent(entityx::Entity e)
    : deadEntity(e)
    {}
    
    entityx::Entity deadEntity;
};

EVENT(DebugEvent)
{
    DebugEvent(std::string m)
    : message(m)
    {}
    
    std::string message;
};

EVENT(GUITextEvent)
{
    GUITextEvent(entityx::Entity e, glm::vec2 p, float s, std::string m)
    : message(m), pos2d(p), entity(e), size(s)
    {}
    
    std::string message;
    entityx::Entity entity;
    float size;
    glm::vec2 pos2d;
};

EVENT(FireWeaponEvent)
{
    FireWeaponEvent(entityx::Entity e)
    : entity(e)
    {}
    
    entityx::Entity entity;
};
#endif
