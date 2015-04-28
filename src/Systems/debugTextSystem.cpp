//
//  debugTextSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/17/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "debugTextSystem.h"
#include "includes.h"
#include "componentTypes.h"

using namespace entityx;

DebugTextSystem::DebugTextSystem(TextRenderer *text)
{
    debugTextPtr = text;
}

void DebugTextSystem::configure(entityx::EventManager &events)
{
    events.subscribe<DebugEvent>(*this);
    events.subscribe<GUITextEvent>(*this);
}
void DebugTextSystem::receive(const DebugEvent &e)
{
    assert(nullptr not_eq debugTextPtr);
    messages.push_back({currentTime, e.message});
}

void DebugTextSystem::receive(const GUITextEvent &e)
{
    entityx::Entity entity = e.entity; //get rid of const-ness

    auto pos2d = e.pos2d;
    
    if (not e.entity.has_component<UIText>()) {
        entity.assign<UIText>(e.size);
    }
    auto UIHandle = entity.component<UIText>();
    auto offset2d = UIHandle->getOffset();

    assert(nullptr not_eq debugTextPtr);
    debugTextPtr->guiText.push_back(Text(pos2d+offset2d,
                                         e.size,
                                         e.message));
}

void DebugTextSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
    assert(debugTextPtr);
    debugTextPtr->guiText.clear();
    
    UIText::Handle text;
    for (auto entity : entities.entities_with_components(text))
    {
        (void) entity;
        text->clearOffset();
    }
    
    currentTime += dt;
    //each message lasts n seconds
    for (auto &message : messages)
    {
        debugTextPtr->debugTexts.push_back(message.message);
    }
    
    //messages come in chronological order
    //delete top messages
    for (auto it = messages.begin(); it != messages.end(); )
    {
        if (it->expirationTime <= currentTime) {
            messages.erase(it);
        } else {
            break;
        }
    }
    
    
}