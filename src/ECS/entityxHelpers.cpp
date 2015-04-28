//
//  entityxHelpers.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/27/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "entityxHelpers.h"
#include "includes.h"
#include "componentTypes.h"
#include "events.h"

void printOE(std::string name, entityx::Entity entity, entityx::EntityManager &entities, entityx::EventManager &events)
{
    auto vp = getViewProjection(entities);
    
    auto pos = entity.component<Position>()->pos;
    assert(pos != glm::vec3());
    auto pos2d = getVec2(vp, pos);
    events.emit<GUITextEvent>(entity, pos2d, 15.0f, name);
};

void printOE(std::string name, float element, entityx::Entity entity, entityx::EntityManager &entities, entityx::EventManager &events)
{
    name = name + to_string_with_precision(element);
    printOE(name, entity, entities, events);
};
