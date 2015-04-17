//
//  debugTextSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/17/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "debugTextSystem.h"

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
    assert(nullptr not_eq debugTextPtr);
    debugTextPtr->guiText.push_back(Text(e.position2d, e.size, e.message));
}

void DebugTextSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
    debugTextPtr->guiText.clear();
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