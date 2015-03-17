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
}
void DebugTextSystem::receive(const DebugEvent &e)
{
    assert(nullptr not_eq debugTextPtr);
    
    debugTextPtr->debugTexts.push_back(e.message);
    std::cout << "got debug message " << e.message << "\n";
}


void DebugTextSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
}