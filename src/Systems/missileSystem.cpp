//
//  missileSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "missileSystem.h"
#include "componentTypes.h"
#include "missileComponent.h"
#include "velocityComponent.h"
#include "includes.h"

#include "vectord.h"
#include "orbitalelements.h"
#include "oeconvert.h"
#include "kepler.h"
#include "lambert.h"
#include "PagmoLambert.h"

#define PAGMO true
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

        
#if 1
        if (missile->done) {
            continue;
        }
        missile->done = true;
        
        Position::Handle targetPosition = missile->target.component<Position>();
        Velocity::Handle targetVelocity = missile->target.component<Velocity>();
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto gm = parentEntity.component<GM>()->gm;
        

        //using lambert equation
        //get my oe
        auto posVel = toPosVelVector(MissilePosition->pos, MissileVelocity->vel);
        auto oe0 = rv2oe(gm, posVel);
        
        // get target location at time...
        auto posVel2 = toPosVelVector(targetPosition->pos, targetVelocity->vel);
        auto oef = rv2oe(gm, posVel2);

        //keep trying w/ smaller dt until a valid iv is found
        glm::vec3 iv;   //injection vector
        double dt2 = 10;
        do{
            std::cout << "dt2: " << dt2 << std::endl;
            oef.tra = anomalyAfterTime(gm, oef, dt2);
            
            auto r0 = oe2r(gm, oe0);
            auto rf = oe2r(gm, oef);
            //debug stuff
//            auto rfPos = glm::vec3(rf[0],
//                                   rf[1],
//                                   rf[2]);
//            auto placeHolderEntity = entities.create();
//            placeHolderEntity.assign<Position>(rfPos);
//            placeHolderEntity.assign<Ship>();
//            placeHolderEntity.assign<Orientation>();
#if PAGMO
            double v1[3], v2[3];
            double a, p, theta;
            int iter;
            //must convert all to double first!
            LambertI(r0.data(), rf.data(), dt2, gm, false,
                     v1, v2, a, p, theta, iter);
            iv.x = v1[0];
            iv.y = v1[1];
            iv.z = v1[2];
#else
            auto velocities = boundingVelocities(gm, r0, rf, dt2, false);
            iv = glm::vec3(velocities[0],
                                   velocities[1],
                                   velocities[2]);
#endif
            dt2 /= 2.0;
            
            if (dt2 < 0.1) {
                std::cout << "BUG: no satisfiable dt found\n";
                break;
            }
        }while(std::isnan(iv.x));
        
        if (dt2 < 0.1) {
            continue;
        }
//        auto iv = glm::vec3(velocities[0],
//                                   velocities[1],
//                                   velocities[2]);
//        std::cout << "inject vel: "
//                  << printVec3(iv)
//                  << std::endl;
        
        //make sure we don't have nan result
        MissileVelocity->vel = iv;
        continue;
#else
        //acceleration should depend on missile engine and mass components
        //probably need to call on Jay's linear dynamics instead of this hacky thing
        if (not missile->target.valid()) {
            entity.remove<MissileLogic>();
            continue;
            //now the missile is dead
        }
        Position::Handle targetPosition = missile->target.component<Position>();
        Velocity::Handle targetVelocity = missile->target.component<Velocity>();
        float spring = 0.1;
        float damper = 0.1;
        glm::vec3 offset_pos = targetPosition->pos - MissilePosition->pos;
        glm::vec3 offset_vel = targetVelocity->vel - MissileVelocity->vel;
        MissileVelocity->vel += glm::normalize(offset_pos) * spring + glm::normalize(offset_vel) * damper;
#endif
    }
}
