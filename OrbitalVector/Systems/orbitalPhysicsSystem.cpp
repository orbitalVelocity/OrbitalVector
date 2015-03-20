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
#if 1
    //OLD!! Just using this to make sure it all works w/ the old system before I integrate the new one
    ComponentHandle<OrbitPath> orbit;
    ComponentHandle<Position> position;
    ComponentHandle<Velocity> velocity;
    ComponentHandle<GM> gm;
   
    for (Entity entity: entities.entities_with_components(orbit, position, gm, velocity))
    {
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto parentPosition = parentEntity.component<Position>();
        auto parentVelocity = parentEntity.component<Velocity>();
        auto parentGM = parentEntity.component<GM>();
        
        //prep this entity for orbit calculation in the old framework (rk547m)
        body parentBody(state(parentPosition->pos, parentVelocity->vel),
                        parentGM->gm,
                        1,
                        nullptr,
                        BodyType::PROJECTILE);
        body selfBody(state(position->pos, velocity->vel),
                      gm->gm,
                      1,
                      nullptr,
                      BodyType::PROJECTILE);
        std::vector<body> newSys;
        newSys.push_back(parentBody);
        newSys.push_back(selfBody);
        float dt2 = dt;
        
        //orbit calculation of each step
        auto orbitPathSteps = 300;
        auto &orbitPath = orbit->path;
        orbitPath.clear();
        orbitPath.reserve(orbitPathSteps*3*2);
        
//        glm::vec3 initialPos = newSys[1].sn.pos;
            orbitPath.push_back(newSys[1].sn.pos.x);
            orbitPath.push_back(newSys[1].sn.pos.y);
            orbitPath.push_back(newSys[1].sn.pos.z);
        
        auto origin = newSys[0].sn.pos;
        for (int i = 0; i < orbitPathSteps; i++) {
            orbitPhysicsUpdate(dt2, ks, newSys, true);
            //reset planet location (lots of errors)
            newSys[0].sn.pos = origin;
            
            orbitPath.push_back(newSys[1].sn.pos.x);
            orbitPath.push_back(newSys[1].sn.pos.y);
            orbitPath.push_back(newSys[1].sn.pos.z);
            orbitPath.push_back(newSys[1].sn.pos.x);
            orbitPath.push_back(newSys[1].sn.pos.y);
            orbitPath.push_back(newSys[1].sn.pos.z);
        }
 
        orbitPath.erase(orbitPath.end()-1);
        orbitPath.erase(orbitPath.end()-1);
        orbitPath.erase(orbitPath.end()-1);
        assert(not orbit->path.empty());
        
    }

    //calculate the next step for all objects
    //legacy crap for all objects
    updateOrbitalPhysics(dt, ks, false);

#else
#endif
}