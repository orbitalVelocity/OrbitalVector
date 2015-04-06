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

#include "vectord.h"
#include "orbitalelements.h"
#include "oeconvert.h"
#include "kepler.h"
#include "lambert.h"

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
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto gm = parentEntity.component<GM>()->gm;
        
        glm::vec3 targetVector = glm::normalize(targetPosition->pos - MissilePosition->pos);
#if 0
        if (missile->done) {
            continue;
        }
        missile->done = true;
        //using lambert equation
        //get my oe
        auto posVel = toPosVelVector(MissilePosition->pos, MissileVelocity->vel);
        auto oe0 = rv2oe(gm, posVel);
        
        // get target location at time...
        auto posVel2 = toPosVelVector(targetPosition->pos, targetVelocity->vel);
        auto oef = rv2oe(gm, posVel2);
        static double dt2 = 0.00000001;
//        dt2 += .5;
        std::cout << "dt2: " << dt2 << std::endl;
        oef.tra = anomalyAfterTime(gm, oef, dt2);
        
        auto r0 = oe2r(gm, oe0);
        auto rf = oe2r(gm, oef);
        auto rfPos = glm::vec3(rf[0],
                               rf[1],
                               rf[2]);
        //debug stuff
        auto placeHolderEntity = entities.create();
        placeHolderEntity.assign<Position>(rfPos);
        placeHolderEntity.assign<Ship>();
        placeHolderEntity.assign<Orientation>();
        
        auto velocities = boundingVelocities(gm, r0, rf, dt2, false);
        auto iv = glm::vec3(velocities[0],
                                   velocities[1],
                                   velocities[2]);
        std::cout << "inject vel: "
                  << printVec3(iv)
                  << std::endl;
        
        //make sure we don't have nan result
        assert(iv.x == iv.x);
        MissileVelocity->vel = iv;
        continue;
#endif
        
        //acceleration should depend on missile engine and mass components
        //probably need to call on Jay's linear dynamics instead of this hacky thing
        float scale = 0.001;
        MissileVelocity->vel += targetVector * scale;
    }
}