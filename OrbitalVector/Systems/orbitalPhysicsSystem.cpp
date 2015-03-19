//
//  orbitalPhysicsSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/18/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "orbitalPhysicsSystem.h"
#include "componentTypes.h"

#include "ecs.h" //FIXME: get rid of ASAP!

using namespace entityx;

void OrbitalPhysicsSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
    ComponentHandle<Position> position;
    ComponentHandle<Velocity> velocity;
    ComponentHandle<GM> gm;
   
#if 1
    //legacy crap
    updateOrbitalPhysics(dt, ks, false);
#else
    for (Entity entity: entities.entities_with_components(position, gm, velocity))
    {
        auto parentPosition = entity.component<Position>();
        auto parentVelocity = entity.component<Velocity>();
        auto parentGM = entity.component<GM>();
        
        //call a function to get stuff
        //update position/velocity of self, ignore effects on parent?
        
        //update orbital elements?
        //errors?
    }
#endif
}