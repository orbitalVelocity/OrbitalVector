//
//  eventScheduleSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/17/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "eventScheduleSystem.h"

//using namespace entityx;
//
//EventScheduleSystem::EventScheduleSystem()
//{
//}
//
//void EventScheduleSystem::configure(entityx::EventManager &events)
//{
//    events.subscribe<DelayedDeathEvent>(*this);
////    events.subscribe<DelayedDebugEvent>(*this);
//    eventsPtr = &events;
//}
////void EventScheduleSystem::receive(const DelayedDebugEvent &e)
////{
////    
////}
//
//void EventScheduleSystem::receive(const DelayedDeathEvent &e)
//{
//    auto futureTime = currentTime+e.delayedms;
//    delayedEvents[futureTime] =
//        [=]()
//        {
//            eventsPtr->emit<DeathEvent>(e.deadEntity);
//        };
//}
//
//
//void EventScheduleSystem::update(EntityManager & entities,
//                             EventManager &events,
//                             double dt)
//{
//    currentTime += dt;
//    for (auto &pair : delayedEvents)
//    {
//        if (pair.first < currentTime) {
//            break;
//        }
//        pair.second();
//    }
//}