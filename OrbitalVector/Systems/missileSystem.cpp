//
//  missileSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "missileSystem.h"
#include "componentTypes.h"
#include "includes.h"

using namespace entityx;

MissileSystem::MissileSystem()
{}

void MissileSystem::update(EntityManager & entities,
                            EventManager &events,
                            double dt)
{
    MissileLogic::Handle missile;
    Position::Handle MissilePosition;
    Velocity::Handle MissileVelocity;
    for (Entity entity : entities.entities_with_components(missile, MissilePosition, MissileVelocity))
    {
        Position::Handle targetPosition = missile->target.component<Position>();
        Velocity::Handle targetVelocity = missile->target.component<Velocity>();
        
        glm::vec3 targetVector = glm::normalize(targetPosition->pos - MissilePosition->pos);
        
        //acceleration should depend on missile engine and mass components
        //probably need to call on Jay's linear dynamics instead of this hacky thing
        float scale = 0.1;
        MissileVelocity->vel += targetVector * scale;
    }
}