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
        path.push_back(sin(rad)*radiusY);
        path.push_back(0);
        path.push_back(cos(rad)*radiusX);
        path.push_back(sin(rad)*radiusY);
        path.push_back(0);
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
    path.push_back(0);
    path.push_back(1e3);
}

VectorD convertToParams (glm::vec3 pos, double gm)
{
    VectorD params(7);
    params[0] = pos.x;
    params[1] = pos.y;
    params[2] = pos.z;
    params[3] = gm;
    params[4] = 0.0;
    params[5] = 0.0;
    params[6] = 0.0;
    return params;
}

std::vector<double> toPosVelVector(glm::vec3 pos, glm::vec3 vel)
{
    std::vector<double> entityStats(6);
    //            VectorD entityStats(6);
    
    entityStats[0] = pos.x;
    entityStats[1] = pos.y;
    entityStats[2] = pos.z;
    entityStats[3] = vel.x;
    entityStats[4] = vel.y;
    entityStats[5] = vel.z;
    return entityStats;
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
   
    auto orbitCount = 0;
    for (Entity entity: entities.entities_with_components(orbit, position, velocity))
    {
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto parentPosition = parentEntity.component<Position>();
        auto parentVelocity = parentEntity.component<Velocity>();
        auto parentGM = parentEntity.component<GM>();
        auto &orbitPath = orbit->path;
        auto orbitPathSteps = 360;
        
        orbitPath.clear();
        orbitPath.reserve(1*orbitPathSteps*3*2); //2 vertices/line segment (3 coord/vertex)
        
        //draw a flat elipse
        std::vector<double> posVel = toPosVelVector(position->pos, velocity->vel);
        auto oe = rv2oe(parentGM->gm, posVel);
        assert(oe.lan == oe.lan);
//        assert(oe.aop == oe.aop);
        
        auto smi = oe.sma * sqrt(1-oe.ecc*oe.ecc);   //semiminor axis
        drawEllipse(orbitPathSteps, orbitPath, oe.sma, smi);
        
        //FIXME: hacks used: negative inc axis, lan+180 offset
        //setup rotational/translation transform for elipse
        auto focus = sqrt(pow(oe.sma, 2) - pow(smi, 2));
        auto translate = glm::translate(glm::mat4(), glm::vec3(focus, 0,0));
        auto aop = glm::rotate(glm::mat4(), (float)(oe.aop*180/M_PI), glm::vec3(0, 0, 1));
        auto inc = glm::rotate(glm::mat4(), (float)((oe.inc*180)/M_PI), glm::vec3(-1, 0, 0));
        auto lan = glm::rotate(glm::mat4(), (float)(oe.lan*180/M_PI+180), glm::vec3(0, 0, 1));
        orbit->transform  = lan * inc * aop * translate;
        
        //calculate next position/velocity for this object
        VectorD params = convertToParams(parentPosition->pos, parentGM->gm);
        
        t_integrator integrator = &rungeKutta4;
        t_dynamics dynamics = &twobody_perturbed;
        posVel = integrator(dynamics, 0, dt, posVel, params);
        position->pos = glm::vec3(posVel[0],
                                  posVel[1],
                                  posVel[2]);
        velocity->vel = glm::vec3(posVel[3],
                                  posVel[4],
                                  posVel[5]);
        assert(not orbit->path.empty());
        
////////DEBUG/////////////////////////////////////
        string message;
#if 1
//        message = "r: " + to_string(position->pos.x)
//        + " " + to_string(position->pos.y)
//        + " " + to_string(position->pos.z);
//        events.emit(DebugEvent(message));
//        
//        message = "v: " + to_string(velocity->vel.x)
//        + " " + to_string(velocity->vel.y)
//        + " " + to_string(velocity->vel.z);
//        events.emit(DebugEvent(message));
        
        message = "sma: " + to_string(oe.sma);
        events.emit(DebugEvent(message));
        message = "ecc: " + to_string(oe.ecc);
        events.emit(DebugEvent(message));
        message = "inc: " + to_string(oe.inc);
        events.emit(DebugEvent(message));
        message = "lan: " + to_string(oe.lan);
        events.emit(DebugEvent(message));
        message = "aop: " + to_string(oe.aop);
        events.emit(DebugEvent(message));
//        message = "tra: " + to_string(oe.tra);
//        events.emit(DebugEvent(message));
#else
        message = "orbits: " + to_string(orbitCount);
        events.emit(DebugEvent(message));
#endif
////////DEBUG/////////////////////////////////////
        
        if (orbitCount++ > 0)
            std::cout << "updating more than one orbit!\n";
    }

}