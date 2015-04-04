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
#include "vectord.h"
#include "integration.h"
#include "twobody.h"

#include "orbitalelements.h"
#include "oeconvert.h"

using namespace entityx;

void drawEllipse(int segments, vector<float> &path, GLdouble radiusX, GLdouble radiusY)
{
    float DEG2RAD = M_PI/180.0f;
    
    //each line segment requires its own vertices
    for(int i=0;i<segments;i++)
    {
        GLdouble rad = i*DEG2RAD;
        path.push_back(cos(rad)*radiusX);
        path.push_back(0);
        path.push_back(sin(rad)*radiusY);
        path.push_back(cos(rad)*radiusX);
        path.push_back(0);
        path.push_back(sin(rad)*radiusY);
    }
    //pull the first vertex out and stuff it in the back
    for(int i=0; i<3; ++i)
    {
        path.push_back(path.front());
        path.erase(path.begin());
    }
    
    //add center for debugging
    for(int i=0; i<3; ++i)
        path.push_back(path[i]);
    for(int i=0; i<6; ++i)
        path.push_back(0);
    path.push_back(0);
    path.push_back(1e3);
    path.push_back(0);
}

void OrbitalPhysicsSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
    /**
     *  Refactor goal: update only based on events
     *      such as new entity event,
     *          detection event, 
     *          velocity change events,
     *          periodic events
     */
    /*
     *  All of this just to compute orbital paths using this system
     */
    ComponentHandle<OrbitPath> orbit;
    ComponentHandle<Position> position;
    ComponentHandle<Velocity> velocity;
    ComponentHandle<GM> gm;
   
    static double overallTime = 0;
    
    for (Entity entity: entities.entities_with_components(orbit, position, gm, velocity))
    {
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto parentPosition = parentEntity.component<Position>();
        auto parentVelocity = parentEntity.component<Velocity>();
        auto parentGM = parentEntity.component<GM>();
        auto &orbitPath = orbit->path;
       
        if (true)
        {
            std::vector<double> entityStats(6);
//            VectorD entityStats(6);
            VectorD params(7);
            
            entityStats[0] = position->pos.x;
            entityStats[1] = position->pos.y;
            entityStats[2] = position->pos.z;
            entityStats[3] = velocity->vel.x;
            entityStats[4] = velocity->vel.y;
            entityStats[5] = velocity->vel.z;
            
            auto oe = rv2oe(parentGM->gm, entityStats);
            std::cout << "oe: \n";
            std::cout << oe.sma << "\n";
            std::cout << oe.ecc << "\n";
            std::cout << oe.inc << "\n";
            std::cout << oe.lan << "\n";
            std::cout << oe.aop << "\n";
            std::cout << oe.tra << "\n";
            
            auto orbitPathSteps = 300;
            orbitPath.clear();
            orbitPath.reserve(2*orbitPathSteps*3*2); //2 vertices (3 coord/vertex)
            
            //draw elipse
            auto smi = oe.sma * sqrt(1-oe.ecc*oe.ecc);   //semiminor axis
            auto focus = sqrt(pow(oe.sma, 2) - pow(smi, 2));
            std::cout << "focus: " << focus << std::endl;
            drawEllipse(360, orbitPath, oe.sma, smi);
            auto translate = glm::translate(glm::mat4(), glm::vec3(-focus, 0,0));
            auto aop = glm::rotate(glm::mat4(), (float)oe.aop, glm::vec3(0, 1, 0));
            auto inc = glm::rotate(glm::mat4(), (float)oe.inc, glm::vec3(1, 0, 0));
            auto lan = glm::rotate(glm::mat4(), (float)oe.lan, glm::vec3(0, 1, 0));
            orbit->transform  = lan * inc * aop * translate;
            break;
            
            params[0] = parentPosition->pos.x;
            params[1] = parentPosition->pos.y;
            params[2] = parentPosition->pos.z;
            params[3] = parentGM->gm;
            params[4] = 0.0;
            params[5] = 0.0;
            params[6] = 0.0;
            
            t_integrator integrator = &rungeKutta4;
            t_dynamics dynamics = &twobody_perturbed;
            
            orbitPath.push_back(entityStats[0]);
            orbitPath.push_back(entityStats[1]);
            orbitPath.push_back(entityStats[2]);
            
            auto projectedTime = overallTime;
            auto dt2 = dt * 100;
            for (int i = 0; i < orbitPathSteps; i++) {
                entityStats = integrator(dynamics, projectedTime, dt2, entityStats, params);
                projectedTime += dt2;
                orbitPath.push_back(entityStats[0]);
                orbitPath.push_back(entityStats[1]);
                orbitPath.push_back(entityStats[2]);
                orbitPath.push_back(entityStats[0]);
                orbitPath.push_back(entityStats[1]);
                orbitPath.push_back(entityStats[2]);
            }
//
            
            
        } else {
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
            
            //extra push_back and erase to produce line segments
            //two points per, resulting in redundant vertices in the middle
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
            
        }
        orbitPath.erase(orbitPath.end()-1);
        orbitPath.erase(orbitPath.end()-1);
        orbitPath.erase(orbitPath.end()-1);
        assert(not orbit->path.empty());
    }

    //calculate the next step for all objects
    //legacy crap for all objects
    updateOrbitalPhysics(entities, dt, ks, false);
}